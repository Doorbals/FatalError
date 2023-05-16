// Fill out your copyright notice in the Description page of Project Settings.

#include "Input/FEInputConfig.h"
#include "GameplayTagContainer.h"
#include "EnhancedInput/Public/InputAction.h"

const UInputAction* UFEInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FTaggedInputAction& TaggedInputAction : NativeInputActions)
	{
		// InputAction이 존재하고, 입력된 Tag와 동일할 때 해당 InputAction을 반환
		if(TaggedInputAction.InputAction && TaggedInputAction.InputTag == InputTag)
		{
			return TaggedInputAction.InputAction;
		}
	}

	return nullptr;
}

const UInputAction* UFEInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FTaggedInputAction& TaggedInputAction : AbilityInputActions)
	{
		// InputAction이 존재하고, 입력된 Tag와 동일할 때 해당 InputAction을 반환
		if(TaggedInputAction.InputAction && TaggedInputAction.InputTag == InputTag)
		{
			return TaggedInputAction.InputAction;
		}
	}

	return nullptr;
}
