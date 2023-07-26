// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/FEAICharacterBase.h"
#include "FEAICharacter_NonCombat.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API AFEAICharacter_NonCombat : public AFEAICharacterBase
{
	GENERATED_BODY()
public:
	AFEAICharacter_NonCombat();

protected:
	virtual void PossessedBy(AController* NewController) override;

	void UpdateAlertCondition(bool InBoolValue);

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UBlackboardData> BBAsset;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UBehaviorTree> BTAsset;

public:
	UPROPERTY(EditAnywhere)
	AActor* PatrolLocation;

	UPROPERTY(EditAnywhere)
	class AFEAlert* AlertActor;
};
