// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Task/BTT_SetIsTargetDetected.h"
#include "AIController.h"
#include "AI/FEAIController.h"
#include "AI/FEAIDetectionInfoInterface.h"

UBTT_SetIsTargetDetected::UBTT_SetIsTargetDetected()
{
	NodeName = FString("Set IsTargetDetected");
}

EBTNodeResult::Type UBTT_SetIsTargetDetected::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = EBTNodeResult::Failed;
	
	AAIController* AIController = Cast<AAIController>(OwnerComp.GetAIOwner());
	if(AIController != nullptr)
	{
		AFEAIController* FEAIController = Cast<AFEAIController>(AIController);
		if(FEAIController != nullptr)
		{
			IFEAIDetectionInfoInterface* AIDetectionInfo = Cast<IFEAIDetectionInfoInterface>(FEAIController);
			if(AIDetectionInfo != nullptr)
			{
				AIDetectionInfo->SetIsTargetDetected(InBoolValue);
				AIDetectionInfo->SetIsHearingSound(InBoolValue);
				Result = EBTNodeResult::Succeeded;
			}
		}
	}
	return Result;
}
