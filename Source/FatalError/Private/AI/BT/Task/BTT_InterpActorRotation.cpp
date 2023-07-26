// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Task/BTT_InterpActorRotation.h"

#include "AIController.h"
#include "AI/FEAICharacterBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Kismet/KismetMathLibrary.h"

UBTT_InterpActorRotation::UBTT_InterpActorRotation()
{
	NodeName = FString("Interp Actor Rotation");
	bTickIntervals = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_InterpActorRotation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = EBTNodeResult::InProgress;
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = Cast<AAIController>(OwnerComp.GetOwner());
	
	if(AIController == nullptr)
	{
		Result = EBTNodeResult::Failed;
	}

	AICharacter = Cast<AFEAICharacterBase>(AIController->GetPawn());
	if(AICharacter == nullptr)
	{
		Result = EBTNodeResult::Failed;
	}

	if(BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		UE_LOG(LogTemp, Warning, TEXT("abc"));
		UObject* KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.SelectedKeyName);
		AActor* ActorValue = Cast<AActor>(KeyValue);
		TargetRotator = UKismetMathLibrary::FindLookAtRotation(AICharacter->GetActorLocation(), ActorValue->GetActorLocation());
	}
	else if(BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		UE_LOG(LogTemp, Warning, TEXT("def"));
		const FVector KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
		TargetRotator = UKismetMathLibrary::FindLookAtRotation(AICharacter->GetActorLocation(), KeyValue);
	}
	UE_LOG(LogTemp, Warning, TEXT("!"));
	return Result;
}

void UBTT_InterpActorRotation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	//SetNextTickTime(NodeMemory, 0.1f);
	
	AICharacter->SetActorRotation(UKismetMathLibrary::RInterpTo(
		 FRotator(0.0f, AICharacter->GetActorRotation().Yaw, 0.0f), TargetRotator, DeltaSeconds, 1.0f));
	
	if(AICharacter->GetActorRotation().Yaw == TargetRotator.Yaw)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
