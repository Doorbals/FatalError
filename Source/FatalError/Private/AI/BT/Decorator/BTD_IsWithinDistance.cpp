// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Decorator/BTD_IsWithinDistance.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

UBTD_IsWithinDistance::UBTD_IsWithinDistance()
{
	NodeName = FString("Is Within Distance");
}

bool UBTD_IsWithinDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();

	if(MyBlackboard != nullptr)
	{
		AActor* TargetActor = Cast<AActor>(MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID()));
		if(TargetActor != nullptr)
		{
			float DistanceFromTarget = (OwnerComp.GetOwner()->GetActorLocation() - TargetActor->GetActorLocation()).Length();
			if(DistanceFromTarget < LimitDistance)
			{
				return true;
			}
		}
	}
	return false;
}
