// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FEAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UFEAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

private:
	virtual void NativeInitializeAnimation() override;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<ACharacter> Character;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsCrouching;
};
