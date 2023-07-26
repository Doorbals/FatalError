// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_SetIsTargetDetected.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UBTT_SetIsTargetDetected : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Condition)
	bool InBoolValue;
	
protected:
	UBTT_SetIsTargetDetected();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
