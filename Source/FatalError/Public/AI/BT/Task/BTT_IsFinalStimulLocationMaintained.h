// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_IsFinalStimulLocationMaintained.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UBTT_IsFinalStimulLocationMaintained : public UBTTaskNode
{
	GENERATED_BODY()

protected:
	UBTT_IsFinalStimulLocationMaintained();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
