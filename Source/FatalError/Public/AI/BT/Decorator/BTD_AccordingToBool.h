// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTD_AccordingToBool.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UBTD_AccordingToBool : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

protected:
	UBTD_AccordingToBool();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
