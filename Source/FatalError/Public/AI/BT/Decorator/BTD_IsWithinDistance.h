// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTD_IsWithinDistance.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UBTD_IsWithinDistance : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Condition)
	float LimitDistance;

protected:
	UBTD_IsWithinDistance();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
