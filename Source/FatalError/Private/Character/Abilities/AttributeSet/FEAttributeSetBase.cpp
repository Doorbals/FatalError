// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/AttributeSet/FEAttributeSetBase.h"
#include "Net/UnrealNetwork.h"

UFEAttributeSetBase::UFEAttributeSetBase()
{
}

// 어트리뷰트의 현재 값이 수정되기 직전에 호출되는 함수. NewValue를 통해 어트리뷰트의 현재 값으로 들어오는 변경사항을 클램프하기에 이상적인 위치이다.
void UFEAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// 해당 함수는 어트리뷰트가 변경될 때마다 호출된다.
	Super::PreAttributeChange(Attribute, NewValue);

	// 최대 값이 변경되면, 현재 값과 최대 값의 비율을 유지하기 위해 현재 값 또한 변경시킨다.
	if (Attribute == GetMaxHealthAttribute()) // Get 함수들은 헤더 파일에서 정의한 매크로에 의해 자동 생성된다.
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMaxEnergyAttribute())
	{
		AdjustAttributeForMaxChange(Energy, MaxEnergy, NewValue, GetEnergyAttribute());
	}
	else if(Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0, MaxHealth.GetBaseValue());
	}
	else if(Attribute == GetEnergyAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 0, MaxEnergy.GetCurrentValue());
	}
	else if (Attribute == GetMoveSpeedAttribute())
	{
		// 이동 속도는 150보다 낮을 수 없고, 1000보다 높을 수 없도록 클램프한다.
		NewValue = FMath::Clamp<float>(NewValue, 150, 1000);
	}
}

// 어트리뷰트의 기본값이 Gameplay Effect에 의해 변경된 이후에만 실행되는 함수.
// 즉각적인 Gameplay Effect에 의해 변경되는 어트리뷰트들에 대한 추가 조작을 위한 함수이다.
// 예를 들면 Health 어트리뷰트에서 최종적인 Damage 메타 어트리뷰트 값을 뺀다. Armor 어트리뷰트가 있다면 Armor에서 먼저 빼고, 이후에 남은 값을 Health에서 뺀다.
// 설계상 Damage 메타 어트리뷰트는 Gameplay Effect에 의해서만 발생하고, 절대 Setter에 의해 발생하지 않는다.
// Energy와 같이 Gameplay Effect에 의해서만 기본 값이 변경되는 다른 어트리뷰트도 여기에서 최대값에 해당하는 어트리뷰트 값으로 클램프될 수 있다.
// 이 함수가 호출되면 어트리뷰트에 대한 변경이 이미 발생한 뒤지만, 아직 클라이언트에 리플리케이션되지 않았기 때문에 여기에서 값을 클램프하면 클라이언트는 클램프 후에만 값을 수신한다.
void UFEAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UFEAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UFEAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFEAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFEAttributeSetBase, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFEAttributeSetBase, Energy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFEAttributeSetBase, MaxEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFEAttributeSetBase, EnergyRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFEAttributeSetBase, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFEAttributeSetBase, MoveSpeed, COND_None, REPNOTIFY_Always);
}

// 최대값 어트리뷰트가 변경될 때 이와 연관된 어트리뷰트의 값을 비례적으로 조정해주는 함수
void UFEAttributeSetBase::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute,
	const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	// 현재의 최대값이 변경된 최대값과 같지 않을 때만 아래 부분 실행
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		// 기존의 (현재값 / 최대값)의 비율을 맞추기 위해 현재값을 변경할 NewDelta를 계산.
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		// 기존의 현재값 어트리뷰트에 NewDelta를 곱해준다.
		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

// https://docs.unrealengine.com/5.1/ko/gameplay-attributes-and-attribute-sets-for-the-gameplay-ability-system-in-unreal-engine/
void UFEAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFEAttributeSetBase, Health, OldHealth);
}

void UFEAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFEAttributeSetBase, MaxHealth, OldMaxHealth);
}

void UFEAttributeSetBase::OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFEAttributeSetBase, HealthRegenRate, OldHealthRegenRate);
}

void UFEAttributeSetBase::OnRep_Energy(const FGameplayAttributeData& OldEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFEAttributeSetBase, Energy, OldEnergy);
}

void UFEAttributeSetBase::OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFEAttributeSetBase, MaxEnergy, OldMaxEnergy);
}

void UFEAttributeSetBase::OnRep_EnergyRegenRate(const FGameplayAttributeData& OldEnergyRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFEAttributeSetBase, EnergyRegenRate, OldEnergyRegenRate);
}

void UFEAttributeSetBase::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFEAttributeSetBase, Armor, OldArmor);
}

void UFEAttributeSetBase::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFEAttributeSetBase, MoveSpeed, OldMoveSpeed);
}