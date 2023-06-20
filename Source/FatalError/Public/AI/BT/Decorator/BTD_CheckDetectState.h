// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FatalError.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTD_CheckDetectState.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UBTD_CheckDetectState : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Condition)
	EFEDetectState DetectStateForCheckCondition;
	
protected:
	UBTD_CheckDetectState();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
