// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/FEAICharacterBase.h"
#include "FEAICharacter_Combat.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API AFEAICharacter_Combat : public AFEAICharacterBase
{
	GENERATED_BODY()

public:
	AFEAICharacter_Combat();

protected:
	virtual void PossessedBy(AController* NewController) override;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UBlackboardData> BBAsset;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UBehaviorTree> BTAsset;

public:
	UPROPERTY(EditAnywhere)
	AActor* PatrolLocation;
};
