// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Service/BTS_CalculateDistance.h"
#include "AIController.h"
#include "VectorTypes.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_CalculateDistance::UBTS_CalculateDistance()
{
	NodeName = TEXT("Calculate Distance");
}

void UBTS_CalculateDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();

	if(MyBlackboard != nullptr)
	{
		AActor* TargetActor = Cast<AActor>(MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID()));
		if(TargetActor != nullptr)
		{
			AAIController* AIController = Cast<AAIController>(OwnerComp.GetOwner());
			if(AIController != nullptr)
			{
				float DistanceFromTarget = (TargetActor->GetActorLocation() - AIController->GetPawn()->GetActorLocation()).Length();
				MyBlackboard->SetValueAsFloat(FName("DistanceFromTarget"), DistanceFromTarget);
			}
		}
	}
}
