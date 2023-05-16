// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FEAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/FEAI.h"

AFEAIController::AFEAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAssetRef(TEXT("/Script/AIModule.BlackboardData'/Game/Characters/AI/BB_FEAICharacter.BB_FEAICharacter'"));
	if(nullptr != BBAssetRef.Object)
	{
		BBAsset = BBAssetRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAssetRef(TEXT("/Script/AIModule.BehaviorTree'/Game/Characters/AI/BT_FEAICharacter.BT_FEAICharacter'"));
	if(nullptr != BTAssetRef.Object)
	{
		BTAsset = BTAssetRef.Object;
	}
}

// AI 시작
void AFEAIController::RunAI()
{
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	// 특정 Blackboard 에셋 사용하여 BlackboardComponent 생성
	if(UseBlackboard(BBAsset, BlackboardPtr))
	{
		Blackboard->SetValueAsVector(BBKEY_HOMEPOS, GetPawn()->GetActorLocation());
		// 특정 비헤이비어 트리 에셋 사용하여 비헤이비어 트리 실행
		bool RunResult = RunBehaviorTree(BTAsset);
		ensure(RunResult);
	}
}

// AI 종료
void AFEAIController::StopAI()
{
	// BrainComponent를 비헤이비어트리 컴포넌트로 Cast
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if(BTComponent)
	{
		// 비헤이비어 트리 종료
		BTComponent->StopTree();
	}
}

void AFEAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	RunAI();
}
