// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_PushAlert.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UBTT_PushAlert : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

protected:
	UBTT_PushAlert();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
