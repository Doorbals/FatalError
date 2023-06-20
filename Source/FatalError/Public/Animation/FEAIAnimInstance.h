// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FatalError.h"
#include "FEAIAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UFEAIAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	EFEAIAnimState AnimState;
};
