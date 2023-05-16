// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Abilities/FEAbilitySystemComponent.h"
#include "Character/Abilities/FEGameplayAbility.h"
#include "Input/FEEnhancedInputComponent.h"

void UFEAbilitySystemComponent::ReceiveDamage(UFEAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}

// 입력된 InputTag와 ActivatableAbilities의 AbilitySpec들을 비교하며 해당 어빌리티가 입력된 Tag를 가지고 있다면
// InputPressedSpecHandles와 InputHeldSpecHandles에 해당 AbilitySpec.Handle 추가
void UFEAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

// 입력된 InputTag와 ActivatableAbilities의 AbilitySpec들을 비교하며 해당 어빌리티가 입력된 Tag를 가지고 있다면
// InputReleasedSpecHandles에 해당 AbilitySpec.Handle 추가 및 InputHeldSpecHandles에서 제거
void UFEAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

// 각각의 Input~SpecHandles에 저장된 AbilitySpecHandle들을 AbilitiesToActivate에 저장하고, 이에 저장된 어빌리티들을 활성화
void UFEAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UFEGameplayAbility* AbilityCDO = CastChecked<UFEGameplayAbility>(AbilitySpec->Ability);
				AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
			}
		}
	}

	// InputPressedSpecHandles에 저장되어 있는 어빌리티들에 대해 InputPressed를 true로 변경하고, Pressed에 대한 입력 이벤트를 전달
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					// Ability가 활성화되어 있으므로 입력 이벤트를 전달합니다.
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UFEGameplayAbility* AbilityCDO = CastChecked<UFEGameplayAbility>(AbilitySpec->Ability);
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}

	// InputReleasedSpecHandles에 저장되어 있는 어빌리티들에 대해 InputPressed를 false로 변경하고, Released에 대한 입력 이벤트를 전달
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
					// Ability가 활성화되어 있으므로 입력 이벤트를 전달합니다.
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}
	ClearAbilityInput();
}

// 모든 핸들 초기화
void UFEAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

