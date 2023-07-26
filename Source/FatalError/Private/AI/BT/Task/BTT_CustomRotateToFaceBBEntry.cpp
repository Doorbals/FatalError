// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BT/Task/BTT_CustomRotateToFaceBBEntry.h"
#include "BehaviorTree/Tasks/BTTask_RotateToFaceBBEntry.h"
#include "GameFramework/Actor.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Rotator.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Animation/FEAIAnimInstance.h"
#include "Animation/FEAnimInstance.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BTT_CustomRotateToFaceBBEntry)

UBTT_CustomRotateToFaceBBEntry::UBTT_CustomRotateToFaceBBEntry()
{
	NodeName = FString("Custom Rotate To Face BBEntry");
}

void UBTT_CustomRotateToFaceBBEntry::PostInitProperties()
{
	Super::PostInitProperties();

	PrecisionDot = FMath::Cos(FMath::DegreesToRadians(Precision));
}

void UBTT_CustomRotateToFaceBBEntry::PostLoad()
{
	Super::PostLoad();

	PrecisionDot = FMath::Cos(FMath::DegreesToRadians(Precision));
}

namespace
{
	FORCEINLINE_DEBUGGABLE float CalculateAngleDifferenceDot(const FVector& VectorA, const FVector& VectorB)
	{
		return (VectorA.IsNearlyZero() || VectorB.IsNearlyZero())
            ? 1.f
            : VectorA.CosineAngle2D(VectorB);
	}
}

EBTNodeResult::Type UBTT_CustomRotateToFaceBBEntry::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (AIController == NULL || AIController->GetPawn() == NULL)
	{
		return EBTNodeResult::Failed;
	}

	FBTFocusMemory* MyMemory = (FBTFocusMemory*)NodeMemory;
	check(MyMemory);
	MyMemory->Reset();

	EBTNodeResult::Type Result = EBTNodeResult::Failed;

	APawn* Pawn = AIController->GetPawn();
	const FVector PawnLocation = Pawn->GetActorLocation();
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();

	FRotator PawnDirectionRotator, ToFocalPointRotator;
	PawnDirectionRotator = Pawn->GetActorForwardVector().Rotation();
	
	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		UObject* KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
		AActor* ActorValue = Cast<AActor>(KeyValue);
		ToFocalPointRotator = (ActorValue->GetActorLocation() - PawnLocation).Rotation();

		if (ActorValue != NULL)
		{
			const float AngleDifference = CalculateAngleDifferenceDot(Pawn->GetActorForwardVector()
				, (ActorValue->GetActorLocation() - PawnLocation));
			if (AngleDifference >= PrecisionDot)
			{
				Result = EBTNodeResult::Succeeded;
			}
			else
			{
				AIController->SetFocus(ActorValue, EAIFocusPriority::Gameplay);
				MyMemory->FocusActorSet = ActorValue;
				MyMemory->bActorSet = true;
				Result = EBTNodeResult::InProgress;
			}
		}
	}
	else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		const FVector KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
		ToFocalPointRotator = (KeyValue - PawnLocation).Rotation();
		
		if (FAISystem::IsValidLocation(KeyValue))
		{
			const float AngleDifference = CalculateAngleDifferenceDot(Pawn->GetActorForwardVector()
				, (KeyValue - PawnLocation));

			if (AngleDifference >= PrecisionDot)
			{
				Result = EBTNodeResult::Succeeded;
			}
			else
			{
				AIController->SetFocalPoint(KeyValue, EAIFocusPriority::Gameplay);
				MyMemory->FocusLocationSet = KeyValue;
				Result = EBTNodeResult::InProgress;
			}
		}
	}
	else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Rotator::StaticClass())
	{
		const FRotator KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Rotator>(BlackboardKey.GetSelectedKeyID());
		ToFocalPointRotator = KeyValue;
		
		if (FAISystem::IsValidRotation(KeyValue))
		{
			const FVector DirectionVector = KeyValue.Vector();
			const float AngleDifference = CalculateAngleDifferenceDot(Pawn->GetActorForwardVector(), DirectionVector);
			
			if (AngleDifference >= PrecisionDot)
			{
				Result = EBTNodeResult::Succeeded;
			}
			else
			{
				const FVector FocalPoint = PawnLocation + DirectionVector * 10000.0f;
				// set focal somewhere far in the indicated direction
				AIController->SetFocalPoint(FocalPoint, EAIFocusPriority::Gameplay);
				MyMemory->FocusLocationSet = FocalPoint;
				Result = EBTNodeResult::InProgress;
			}
		}
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
	
	return Result;
}

void UBTT_CustomRotateToFaceBBEntry::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (AIController == NULL || AIController->GetPawn() == NULL)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	else
	{
		const FVector PawnDirection = AIController->GetPawn()->GetActorForwardVector();
		const FVector FocalPoint = AIController->GetFocalPointForPriority(EAIFocusPriority::Gameplay);
		if (FocalPoint != FAISystem::InvalidLocation)
		{
			if (CalculateAngleDifferenceDot(PawnDirection, FocalPoint - AIController->GetPawn()->GetActorLocation()) >= PrecisionDot)
			{
				const float CurrentAngleRadians = CalculateAngleDifferenceDot(PawnDirection, (FocalPoint - AIController->GetPawn()->GetActorLocation()));
				CleanUp(*AIController, NodeMemory);
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
		}
		else
		{
			CleanUp(*AIController, NodeMemory);
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
	}
}

void UBTT_CustomRotateToFaceBBEntry::CleanUp(AAIController& AIController, uint8* NodeMemory)
{
	FBTFocusMemory* MyMemory = (FBTFocusMemory*)NodeMemory;
	check(MyMemory);
	
	bool bClearFocus = false;
	if (MyMemory->bActorSet)
	{
		bClearFocus = (MyMemory->FocusActorSet == AIController.GetFocusActorForPriority(EAIFocusPriority::Gameplay));
	}
	else
	{
		bClearFocus = (MyMemory->FocusLocationSet == AIController.GetFocalPointForPriority(EAIFocusPriority::Gameplay));
	}
	
	if (bClearFocus)
	{
		AIController.ClearFocus(EAIFocusPriority::Gameplay);
	}
}

EBTNodeResult::Type UBTT_CustomRotateToFaceBBEntry::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (AIController != NULL)
	{
		CleanUp(*AIController, NodeMemory);
	}

	return EBTNodeResult::Aborted;
}

void UBTT_CustomRotateToFaceBBEntry::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	FString KeyDesc = BlackboardKey.SelectedKeyName.ToString();
	Values.Add(FString::Printf(TEXT("%s: %s"), *Super::GetStaticDescription(), *KeyDesc));

	AAIController* AIController = OwnerComp.GetAIOwner();

	if (AIController != NULL && AIController->GetPawn() != NULL)
	{
		const FVector PawnDirection = AIController->GetPawn()->GetActorForwardVector();
		const FVector FocalPoint = AIController->GetFocalPointForPriority(EAIFocusPriority::Gameplay);

		if (FocalPoint != FAISystem::InvalidLocation)
		{
			const float CurrentAngleRadians = CalculateAngleDifferenceDot(PawnDirection, (FocalPoint - AIController->GetPawn()->GetActorLocation()));
			Values.Add(FString::Printf(TEXT("Current angle: %.2f"), FMath::RadiansToDegrees(FMath::Acos(CurrentAngleRadians))));
		}
		else
		{
			Values.Add(TEXT("FocalPoint is an Invalid Location"));
		}
	}
	else
	{
		Values.Add(TEXT("Controller or Pawn is NULL"));
	}
}

FString UBTT_CustomRotateToFaceBBEntry::GetStaticDescription() const
{
	FString KeyDesc = BlackboardKey.SelectedKeyName.ToString();
	return FString::Printf(TEXT("Target : %s"), *KeyDesc);
} 

