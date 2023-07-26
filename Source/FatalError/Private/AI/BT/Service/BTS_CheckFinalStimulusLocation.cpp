// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Service/BTS_CheckFinalStimulusLocation.h"
#include "AI/FEAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_CheckFinalStimulusLocation::UBTS_CheckFinalStimulusLocation()
{
	NodeName = TEXT("Check Final Stimulus Location");
}

void UBTS_CheckFinalStimulusLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
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
			}
		}
	}
}
