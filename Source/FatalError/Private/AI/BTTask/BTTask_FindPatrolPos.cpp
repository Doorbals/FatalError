// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask/BTTask_FindPatrolPos.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "AI/FEAI.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/FEAICharacterInterface.h"

UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	// 해당 비헤이비어트리 컴포넌트를 가지고 있는 Pawn을 가져오기
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if(nullptr == ControllingPawn)
	{
		// Pawn이 유효하지 않으면 Failed 반환
		return EBTNodeResult::Failed;
	}

	// Pawn이 존재하는 월드에서 네비게이션 시스템을 가져온다.
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
	if(nullptr == NavSystem)
	{
		// 네비게이션 시스템이 유효하지 않으면 Failed 반환
		return EBTNodeResult::Failed;
	}

	IFEAICharacterInterface* AIPawn = Cast<IFEAICharacterInterface>(ControllingPawn);
	if(nullptr == AIPawn)
	{
		return EBTNodeResult::Failed;
	}
	
	// BBKEY_HOMEPOS를 Origin에 저장
	FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(BBKEY_HOMEPOS);
	FNavLocation NextPatrolPos;
	// AI에 지정되어 있는 PatrolRadius 가져와서 사용
	float PatrolRadius = AIPawn->GetAIPatrolRadius();
	
	// 원점에서부터 PatrolRadius 반경의 랜덤 위치를 NextPatrolPos에 저장하고, BBKEY_PATROLPOS에 NextPatrolPos의 Location 저장
	if(NavSystem->GetRandomPointInNavigableRadius(Origin, PatrolRadius, NextPatrolPos))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(BBKEY_PATROLPOS, NextPatrolPos.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
