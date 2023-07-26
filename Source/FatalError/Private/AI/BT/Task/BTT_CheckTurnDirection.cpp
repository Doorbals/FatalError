// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Task/BTT_CheckTurnDirection.h"
#include "AI/FEAIController.h"
#include "Animation/FEAIAnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "GameFramework/Character.h"

UBTT_CheckTurnDirection::UBTT_CheckTurnDirection()
{
	NodeName = FString("Check Turn Direction");
}

EBTNodeResult::Type UBTT_CheckTurnDirection::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = EBTNodeResult::Failed ;
	UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	
	if(AIController != nullptr)
	{
		APawn* Pawn = AIController->GetPawn();
		const FVector PawnLocation = Pawn->GetActorLocation();

		FRotator PawnDirectionRotator, ToFocalPointRotator;
		PawnDirectionRotator = Pawn->GetActorForwardVector().Rotation();
	
		if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
		{
			UObject* KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
			AActor* ActorValue = Cast<AActor>(KeyValue);
			ToFocalPointRotator = (ActorValue->GetActorLocation() - PawnLocation).Rotation();
		}
		else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
			const FVector KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
			ToFocalPointRotator = (KeyValue - PawnLocation).Rotation();
		}

		bool TurnLeft;
		float RotDifference = PawnDirectionRotator.Yaw - ToFocalPointRotator.Yaw;
		if((RotDifference > 0 && RotDifference <= 180) || RotDifference < -180)
		{
			TurnLeft = true;
		}
		else
		{
			TurnLeft = false;
		}
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(FName("TurnLeft"), TurnLeft);
	
		ACharacter* Character = Cast<ACharacter>(Pawn);
		if (Character != nullptr)
		{
			UFEAIAnimInstance* AnimInstance = Cast<UFEAIAnimInstance>(Character->GetMesh()->GetAnimInstance());
			AnimInstance->TurnLeft = TurnLeft;
		}
		
		Result = EBTNodeResult::Succeeded;
	}
	return Result;
}
