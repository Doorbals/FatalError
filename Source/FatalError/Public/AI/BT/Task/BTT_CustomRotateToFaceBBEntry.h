// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_RotateToFaceBBEntry.h"
#include "BTT_CustomRotateToFaceBBEntry.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UBTT_CustomRotateToFaceBBEntry : public UBTTask_RotateToFaceBBEntry
{
	GENERATED_BODY()

protected:
	/** Success condition precision in degrees */

private:
	/** cached Precision tangent value */
	float PrecisionDot;

public:
	UBTT_CustomRotateToFaceBBEntry();
	virtual void PostInitProperties() override;
	virtual void PostLoad() override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;
	virtual FString GetStaticDescription() const override;
	
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FBTFocusMemory); }

protected:

	float GetPrecisionDot() const { return PrecisionDot; }
	void CleanUp(AAIController& AIController, uint8* NodeMemory);
	
};
