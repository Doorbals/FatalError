// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Task/BTT_ChangeMoveSpeed.h"

#include "AI/FEAICharacterBase.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTT_ChangeMoveSpeed::UBTT_ChangeMoveSpeed()
{
	NodeName = FString("Change Move Speed");
}

EBTNodeResult::Type UBTT_ChangeMoveSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = EBTNodeResult::Failed;
	UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	
	AAIController* AIController = Cast<AAIController>(OwnerComp.GetAIOwner());
	if(AIController != nullptr)
	{
		AFEAICharacterBase* AICharacter = Cast<AFEAICharacterBase>(AIController->GetPawn());
		if(AICharacter != nullptr && MyBlackboard->GetValueAsFloat(FName("CurrentMoveSpeed")) != SetMoveSpeed)
		{
			AICharacter->GetCharacterMovement()->MaxWalkSpeed = SetMoveSpeed;
			MyBlackboard->SetValueAsFloat(FName("CurrentMoveSpeed"), SetMoveSpeed);
		}
		Result = EBTNodeResult::Succeeded;
	}
	
	return Result;
}
