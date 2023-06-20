// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Task/BTT_SwitchBool.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

UBTT_SwitchBool::UBTT_SwitchBool()
{
	NodeName = FString("Switch Bool");
}

EBTNodeResult::Type UBTT_SwitchBool::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = EBTNodeResult::Failed ;
    UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();

	if(IsValid(MyBlackboard))
	{
		MyBlackboard->SetValue<UBlackboardKeyType_Bool>(BlackboardKey.GetSelectedKeyID(), BoolValue);
		Result = EBTNodeResult::Succeeded;
	}
	
    return Result;
}
