// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "FatalError.h"
#include "BTT_ChangeAnimState.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UBTT_ChangeAnimState : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Condition)
	EFEAIAnimState SetAIAnimState;

protected:
	UBTT_ChangeAnimState();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
