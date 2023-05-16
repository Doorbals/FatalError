// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "FEInputConfig.h"
#include "GameplayTagContainer.h"
#include "FEEnhancedInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UFEEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:
	template<class UserClass, typename FuncType>
	void BindNativeActions(const UFEInputConfig* InputConfig, const FGameplayTag InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UFEInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	void RemoveBinds(TArray<uint32>& BindHandles);
};

// NativeInputActions 중에서 입력된 Tag와 Mapping 되어있는 InputAction을 찾아서 해당 InputAction을 바인딩하는 함수
template <class UserClass, typename FuncType>
void UFEEnhancedInputComponent::BindNativeActions(const UFEInputConfig* InputConfig, const FGameplayTag InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func)
{
	check(InputConfig);
	if(const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}

// AbilityInputActions 중에서 입력된 Tag와 Mapping 되어있는 InputAction을 찾아서 해당 InputAction을 바인딩하는 함수
template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UFEEnhancedInputComponent::BindAbilityActions(const UFEInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	for(const FTaggedInputAction& Action : InputConfig->AbilityInputActions)
	{
		if(Action.InputAction && Action.InputTag.IsValid())
		{
			if(PressedFunc)
			{
				// BindAction을 통해 해당 Input이 눌렸을 때 PressedFunc 함수가 실행되도록 바인딩하고, 이 바인딩에 대한 Handle을 BindHandles에 추가한다.
				// (핸들은 필요한 경우 나중에 바인딩을 수정하거나 제거하기 위함)
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
			}

			if(ReleasedFunc)
			{
				// BindAction을 통해 해당 Input이 완료 됐을 때 ReleasedFunc 함수가 실행되도록 바인딩하고, 이 바인딩에 대한 Handle을 BindHandles에 추가한다.
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
			}
		}
	}
}
