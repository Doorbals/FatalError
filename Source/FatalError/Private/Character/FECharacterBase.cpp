// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FECharacterBase.h"
#include "Character/Abilities/FEAbilitySystemComponent.h"
#include "Character/Abilities/FEGameplayAbility.h"
#include "Character/Abilities/AttributeSet/FEAttributeSetBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

class UFEGameplayAbility;

// 생성자 코드
AFECharacterBase::AFECharacterBase(const FObjectInitializer& ObjectInitializer) 
{
	// 해당 액터가 틱 처리를 할 수 있는지 여부 설정.
	PrimaryActorTick.bCanEverTick = false;

	// 액터의 캡슐 컴포넌트가 ECC_Visibility 채널과 상호작용할 때 Overlap 형태의 충돌 응답 하도록 설정
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
	// 서버에서 항상 관련된 상태 유지. false일 경우 클라이언트에서만 유지. (멀티플레이어 게임에서 사용되는 듯?)
	bAlwaysRelevant = true;

	// RequestGameplayTag()로 게임플레이 태그를 생성하여 변수에 할당
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("Effect.RemoveOnDeath"));
}

UAbilitySystemComponent* AFECharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

bool AFECharacterBase::IsAlive() const
{
	return GetHealth() > 0.0f;
}

int32 AFECharacterBase::GetAbilityLevel(EFEAbilityInputID AbilityID) const
{
	return 1;
}

// 캐릭터의 어빌리티 제거하는 함수
void AFECharacterBase::RemoveCharacterAbilities()
{
	// GetLocalRole() : 서버가 아닌 클라이언트에서 실행 중인지 확인 (서버에서 실행 -> ROLE_Authority / 클라이언트에서 실행 : ROLE_SimulatedProxy 반환)
	// 즉, 서버에서 실행되지 않거나, AbilitySystemComponent가 유효하지 않거나, 캐릭터의 어빌리티가 이미 제거된 경우 함수 종료
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || !AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	// 제거할 어빌리티 목록
	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	// 현재 캐릭터에 추가된 활성화 가능한 어빌리티 목록을 가져와서, 목록 중 CharacterAbilities 배열에 있는 어빌리티들만 AbilitiesToRemove에 추가
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		// 어빌리티의 소스 오브젝트가 현재 캐릭터인지 확인(여러 캐릭터가 하나의 어빌리티 공유하는 경우 해당 캐릭터 어빌리티만 제거하기 위함)
		if ((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	// AbilitiesToRemove 배열에 있는 어빌리티들 ClearAbility()로 제거
	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}

	// 어빌리티가 제거되었음을 표시
	AbilitySystemComponent->bCharacterAbilitiesGiven = false;
}

float AFECharacterBase::GetHealth() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetHealth();
	}

	return 0.0f;
}

float AFECharacterBase::GetMaxHealth() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxHealth();
	}

	return 0.0f;
}

float AFECharacterBase::GetEnergy() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetEnergy();
	}

	return 0.0f;
}

float AFECharacterBase::GetMaxEnergy() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMaxEnergy();
	}

	return 0.0f;
}

float AFECharacterBase::GetMoveSpeed() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMoveSpeed();
	}

	return 0.0f;
}

float AFECharacterBase::GetMoveSpeedBaseValue() const
{
	if (AttributeSetBase.IsValid())
	{
		return AttributeSetBase->GetMoveSpeedAttribute().GetGameplayAttributeData(AttributeSetBase.Get())->GetBaseValue();
	}

	return 0.0f;
}

// 캐릭터가 죽었을 때 실행
void AFECharacterBase::Die()
{
	// 캐릭터의 어빌리티 모두 제거
	RemoveCharacterAbilities();

	// 캡슐 컴포넌트의 충돌 비활성화 및 캐릭터의 움직임 중지
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->Velocity = FVector(0);
	
	OnCharacterDied.Broadcast(this);
	
	if (AbilitySystemComponent.IsValid())
	{
		// 활성화된 모든 어빌리티 취소
		AbilitySystemComponent->CancelAllAbilities();

		// 게임플레이 태그 컨테이너인 EffectTagsToRemove에 EffectRemoveOnDeathTag 추가
		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		// EffectTagsToRemove에 추가한 게임 플레이 태그 가지고 있는 모든 활성화된 이펙트를 제거
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);
		
		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

	// 설정된 사망 애니메이션 몽타주 있는 경우 실행, 아니면 Destroy
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	else
	{
		FinishDying();
	}
}

void AFECharacterBase::FinishDying()
{
	Destroy();
}

// Called when the game starts or when spawned
void AFECharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

// 캐릭터에게 어빌리티를 부여
void AFECharacterBase::AddCharacterAbilities()
{
	// 서버에서 실행되고, 어빌리티 시스템 컴포넌트가 유효하고, 아직 어빌리티가 주어지지 않은 상태일 때만 아래 내용 실행 
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	// CharacterAbilities 배열을 순회하면서 GiveAbility()로 각 어빌리티를 캐릭터에 추가
	for (TSubclassOf<UFEGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this);
		AbilitySpec.DynamicAbilityTags.AppendTags(AbilitySpec.Ability->AbilityTags);
		 
		AbilitySystemComponent->GiveAbility(AbilitySpec);
	}

	// 어빌리티가 추가되었음을 표시
	AbilitySystemComponent->bCharacterAbilitiesGiven = true;
}

// 캐릭터가 스폰될 때 즉시 캐릭터의 어트리뷰트를 초기화
void AFECharacterBase::InitializeAttributes()
{
	if (!AbilitySystemComponent.IsValid())
	{
		return;
	}
	
	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}

	// EffectContext(게임 플레이 이펙트와 관련된 정보 저장)를 생성하고, 그 소스 오브젝트를 현재 캐릭터 객체로 지정
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	// 이 부분 잘 모르겠다!
	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, 0, EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
	}
}

void AFECharacterBase::AddStartupEffects()
{
	// 서버에서 실행되고, 어빌리티 시스템 컴포넌트가 유효하고, 아직 어빌리티가 주어지지 않은 상태일 때만 아래 내용 실행 
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->bStartupEffectsApplied)
	{
		return;
	}

	// 이 부분도 잘 모르겠다..!
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 0, EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}

	AbilitySystemComponent->bStartupEffectsApplied = true;
}

void AFECharacterBase::SetHealth(float Health)
{
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetHealth(Health);
	}
}

void AFECharacterBase::SetEnergy(float Energy)
{
	if (AttributeSetBase.IsValid())
	{
		AttributeSetBase->SetEnergy(Energy);
	}
}



