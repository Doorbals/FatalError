// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FEPlayerState.h"
#include "Character/Abilities/FEAbilitySystemComponent.h"
#include "Character/Abilities/AttributeSet/FEAttributeSetBase.h"

AFEPlayerState::AFEPlayerState()
{
	// 어빌리티 시스템 컴포넌트를 생성하여 하위 오브젝트로 추가하고, 리플리케이트 되도록 설정
	AbilitySystemComponent = CreateDefaultSubobject<UFEAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// 어빌리티 시스템 컴포넌트에 리플리케이트 모드 설정
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	// AttributeSetBase를 생성하여 하위 오브젝트로 추가
	AttributeSetBase = CreateDefaultSubobject<UFEAttributeSetBase>(TEXT("AttributeSetBase"));

	// 업데이트 주기 설정
	NetUpdateFrequency = 100.0f;

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

UAbilitySystemComponent* AFEPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UFEAttributeSetBase* AFEPlayerState::GetAttributeSetBase()
{
	return AttributeSetBase;
}

bool AFEPlayerState::IsAlive()
{
	return GetHealth() > 0.0f;
}

float AFEPlayerState::GetHealth() const
{
	return AttributeSetBase->GetHealth();
}

float AFEPlayerState::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}

float AFEPlayerState::GetHealthRegenRate() const
{
	return AttributeSetBase->GetHealthRegenRate();
}

float AFEPlayerState::GetEnergy() const
{
	return AttributeSetBase->GetEnergy();
}

float AFEPlayerState::GetMaxEnergy() const
{
	return AttributeSetBase->GetMaxEnergy();
}

float AFEPlayerState::GetEnergyRegenRate() const
{
	return AttributeSetBase->GetEnergyRegenRate();
}

float AFEPlayerState::GetArmor() const
{
	return AttributeSetBase->GetArmor();
}

float AFEPlayerState::GetMoveSpeed() const
{
	return AttributeSetBase->GetMoveSpeed();
}

void AFEPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		// 어트리뷰트가 변경될 때마다 실행되는 콜백 함수들을 각 델리게이트 핸들에 바인딩
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AFEPlayerState::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxHealthAttribute()).AddUObject(this, &AFEPlayerState::MaxHealthChanged);
		HealthRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthRegenRateAttribute()).AddUObject(this, &AFEPlayerState::HealthRegenRateChanged);
		EnergyChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetEnergyAttribute()).AddUObject(this, &AFEPlayerState::EnergyChanged);
		MaxEnergyChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxEnergyAttribute()).AddUObject(this, &AFEPlayerState::MaxEnergyChanged);
		EnergyRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetEnergyRegenRateAttribute()).AddUObject(this, &AFEPlayerState::EnergyRegenRateChanged);
	}
}

void AFEPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("Health Changed %f"), Data.NewValue);
	HealthChangeDelegate.Execute(Data.NewValue / GetMaxHealth());
	
	if(Data.NewValue <= 0.0f && Data.OldValue != 0.0f)
	{
		RunOutOfHealthDelegate.Execute();
	}
}

void AFEPlayerState::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("MaxHealth Changed"));
}

void AFEPlayerState::HealthRegenRateChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("HealthRegen Changed"));
}

void AFEPlayerState::EnergyChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("Energy Changed"));
	EnergyChangeDelegate.Execute(Data.NewValue / GetMaxEnergy());
}

void AFEPlayerState::MaxEnergyChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("MaxEnergy Changed"));
}

void AFEPlayerState::EnergyRegenRateChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("EnergyRegen Changed"));
}

void AFEPlayerState::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		// Ability 태그 가진 어빌리티를 취소
		FGameplayTagContainer AbilityTagsToCancel;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));

		// Ability.NotCanceldByStun 태그 가진 어빌리티는 취소에서 제외
		FGameplayTagContainer AbilityTagsToIgnore;
		AbilityTagsToIgnore.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCanceledByStun")));

		// 즉, Ability 태그 가진 어빌리티 중 NotCanceldByStun 태그가 붙은 어빌리티 제외하고 전부 취소
		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);
	}
}
