// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Decorator/BTD_AccordingToBool.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

UBTD_AccordingToBool::UBTD_AccordingToBool()
{
	NodeName = FString("According To Bool");
}

bool UBTD_AccordingToBool::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
    
	if(MyBlackboard != nullptr)
	{
		bool BoolValue = MyBlackboard->GetValue<UBlackboardKeyType_Bool>(BlackboardKey.GetSelectedKeyID()); 
		if(BoolValue)
		{
			return true;
		}
	}
	return false;
}
