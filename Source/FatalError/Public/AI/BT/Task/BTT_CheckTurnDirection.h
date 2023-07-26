// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_CheckTurnDirection.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UBTT_CheckTurnDirection : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

protected:
	UBTT_CheckTurnDirection();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
