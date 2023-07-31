// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BT/Task/BTT_PushAlert.h"
#include "AI/FEAlert.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_PushAlert::UBTT_PushAlert()
{
	NodeName = FString("Push Alert");
}

EBTNodeResult::Type UBTT_PushAlert::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = EBTNodeResult::Failed ;
	UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	
	AFEAlert* AlertActor = Cast<AFEAlert>(MyBlackboard->GetValueAsObject(FName("AlertActor")));
	if(AlertActor != nullptr)
	{
		AlertActor->PushAlert();
		
		Result = EBTNodeResult::Succeeded;
	}
	
	return Result;
}
