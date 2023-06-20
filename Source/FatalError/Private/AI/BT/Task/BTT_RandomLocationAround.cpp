// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Task/BTT_RandomLocationAround.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"


UBTT_RandomLocationAround::UBTT_RandomLocationAround()
{
	NodeName = FString("Random Location Around");
}

EBTNodeResult::Type UBTT_RandomLocationAround::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = EBTNodeResult::Failed;

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if(ControllingPawn != nullptr)
	{
		const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
		UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
		FNavLocation NextLocation;
	
		if(MyBlackboard != nullptr && NavSystem != nullptr)
		{
			// OriginLocationKey의 벡터 값을 원점으로 하여 주변 원 반경의 랜덤 위치를 OriginLocation에 저장
			FVector OriginLocation = MyBlackboard->GetValueAsVector(OriginLocationKey.SelectedKeyName);
			if(NavSystem->GetRandomPointInNavigableRadius(OriginLocation, 1000.0f, NextLocation))
			{
				// BlackboardKey의 값을 NextLocation 값으로 설정
				OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), NextLocation.Location);
				Result = EBTNodeResult::Succeeded;
			}
		}
	}
	
	return Result;
}
