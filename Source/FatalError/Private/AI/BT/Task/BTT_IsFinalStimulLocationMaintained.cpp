// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Task/BTT_IsFinalStimulLocationMaintained.h"
#include "AI/FEAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_IsFinalStimulLocationMaintained::UBTT_IsFinalStimulLocationMaintained()
{
	NodeName = FString("Final Stimulus Location Is Maintained");
}

EBTNodeResult::Type UBTT_IsFinalStimulLocationMaintained::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = EBTNodeResult::Failed ;
	UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	if(AIController != nullptr)
	{
		AFEAIController* FEAIController = Cast<AFEAIController>(AIController);
		if(FEAIController != nullptr)
		{
			IFEAIDetectionInfoInterface* DetectionInfo = Cast<IFEAIDetectionInfoInterface>(AIController);
			if(DetectionInfo != nullptr)
			{
				FVector CurLocation = DetectionInfo->GetFinalStimulusLocation();
				FVector PrevLocation = MyBlackboard->GetValueAsVector(FName("FinalStimulusLocation"));

				if(CurLocation != PrevLocation)
				{
					MyBlackboard->SetValueAsVector(FName("FinalStimulusLocation"), CurLocation);
					MyBlackboard->SetValueAsBool(FName("IsStimulated"), true);
				}
				else
				{
					Result = EBTNodeResult::Succeeded;
				}
			}
		}
	}

	return Result;
}
