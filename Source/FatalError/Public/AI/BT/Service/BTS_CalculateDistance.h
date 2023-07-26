// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_CalculateDistance.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UBTS_CalculateDistance : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
protected:
	UBTS_CalculateDistance();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
