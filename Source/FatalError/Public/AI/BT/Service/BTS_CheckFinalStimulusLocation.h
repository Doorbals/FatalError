// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_CheckFinalStimulusLocation.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UBTS_CheckFinalStimulusLocation : public UBTService
{
	GENERATED_BODY()

protected:
	UBTS_CheckFinalStimulusLocation();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
