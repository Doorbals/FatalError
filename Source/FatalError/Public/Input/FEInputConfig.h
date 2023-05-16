// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "FEInputConfig.generated.h"

class UInputAction;
struct FGameplayTag;
/**
 * 
 */

// InputAction에 GameplayTag 를 Mapping 하는 FTaggedInputAction 구조체 정의
USTRUCT(BlueprintType)
struct FTaggedInputAction
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	const UInputAction* InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

UCLASS()
class FATALERROR_API UFEInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	// 입력된 태그와 연결된 NativeInputAction 반환
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag) const;

	// 입력된 태그와 연결된 AbilityInputAction 반환
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag) const;
	
	// NativeInputAction들을 저장하는 배열. 해당 InputAction들은 GameplayTag와 연결되어 있다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FTaggedInputAction> NativeInputActions;

	// AbilityInputAction들을 저장하는 배열. 해당 InputAction들은 GameplayTag와 연결되어 있다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FTaggedInputAction> AbilityInputActions;
};
