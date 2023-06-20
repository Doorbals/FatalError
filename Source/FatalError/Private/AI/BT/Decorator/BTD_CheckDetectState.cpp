// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Decorator/BTD_CheckDetectState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"

UBTD_CheckDetectState::UBTD_CheckDetectState()
{
	NodeName = FString("Check Detect State");
}

bool UBTD_CheckDetectState::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
    
    if(MyBlackboard != nullptr)
    {
    	uint8 DetectState = MyBlackboard->GetValue<UBlackboardKeyType_Enum>(BlackboardKey.GetSelectedKeyID()); 
    	if(DetectState == static_cast<uint8>(DetectStateForCheckCondition))
    	{
			return true;
    	}
    }
    return false;
}
