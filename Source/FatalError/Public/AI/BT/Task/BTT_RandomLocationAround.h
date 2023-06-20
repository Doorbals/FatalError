// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_RandomLocationAround.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UBTT_RandomLocationAround : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Condition)
	FBlackboardKeySelector OriginLocationKey;
	
protected:
	UBTT_RandomLocationAround();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
