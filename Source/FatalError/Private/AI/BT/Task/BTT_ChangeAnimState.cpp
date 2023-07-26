// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Task/BTT_ChangeAnimState.h"
#include "AIController.h"
#include "AI/FEAICharacterBase.h"
#include "Animation/FEAIAnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

UBTT_ChangeAnimState::UBTT_ChangeAnimState()
{
	NodeName = FString("Change Anim State");
}

EBTNodeResult::Type UBTT_ChangeAnimState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = EBTNodeResult::Failed ;
    UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	
	AAIController* AIController = Cast<AAIController>(OwnerComp.GetAIOwner());
	if(AIController != nullptr)
	{
		AFEAICharacterBase* AICharacter = Cast<AFEAICharacterBase>(AIController->GetPawn());
		if(AICharacter != nullptr)
		{
			UFEAIAnimInstance* AIAnimInstance = Cast<UFEAIAnimInstance>(AICharacter->GetMesh()->GetAnimInstance());
			if(AIAnimInstance != nullptr && MyBlackboard->GetValueAsEnum(FName("CurrentAnimState")) != static_cast<uint8>(SetAIAnimState))
			{
				AIAnimInstance->AnimState = SetAIAnimState;
				MyBlackboard->SetValueAsEnum(FName("CurrentAnimState"), static_cast<uint8>(SetAIAnimState));
				AIAnimInstance->RandomNumber = UKismetMathLibrary::RandomInteger(3);
			}
			Result = EBTNodeResult::Succeeded;
		}
	}
	return Result;
}
