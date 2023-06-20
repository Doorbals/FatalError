// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_ChangeMoveSpeed.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UBTT_ChangeMoveSpeed : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Condition)
	float SetMoveSpeed;

protected:
	UBTT_ChangeMoveSpeed();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
