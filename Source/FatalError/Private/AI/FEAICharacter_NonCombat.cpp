// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FEAICharacter_NonCombat.h"
#include "AI/FEAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

AFEAICharacter_NonCombat::AFEAICharacter_NonCombat()
{
}

void AFEAICharacter_NonCombat::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AFEAIController* AIController = Cast<AFEAIController>(NewController);

	if(AIController)
	{
		// AIController의 BB, BT를 해당 클래스의 BB, BT로 설정
		AIController->BBAsset = BBAsset;
		AIController->BTAsset = BTAsset;
	}
	
	// 비헤이비어 트리 실행
	AIController->RunAI();

	// 해당 AI 캐릭터의 Blackboard의 키 값인 OriginLocation을 시작 위치로 설정
	AIController->GetBlackboardComponent()->SetValueAsVector(FName("OriginLocation"), GetActorLocation());

	// 해당 AI 캐릭터의 시작 위치를 Blackboard의 키 값인 PatrolLocation을 월드 내의 특정 액터 위치로 지정
	if(PatrolLocation != nullptr)
	{
		AIController->GetBlackboardComponent()->SetValueAsVector(FName("PatrolLocation"), PatrolLocation->GetActorLocation());
	}
	
	AIController->GetBlackboardComponent()->SetValueAsEnum(FName("CurrentAnimState"), static_cast<uint8>(EFEAIAnimState::Idle));

	AIController->GetBlackboardComponent()->SetValueAsEnum(FName("CurrentDetectState"), static_cast<uint8>(EFEDetectState::Green));
}
	
