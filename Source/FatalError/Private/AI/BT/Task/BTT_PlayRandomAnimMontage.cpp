// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Task/BTT_PlayRandomAnimMontage.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

UBTT_PlayRandomAnimMontage::UBTT_PlayRandomAnimMontage()
{
	NodeName = FString("Play Random Anim Montage");
	bTickIntervals = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_PlayRandomAnimMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = EBTNodeResult::InProgress;

	AAIController* AIController = Cast<AAIController>(OwnerComp.GetOwner());
	if(AIController == nullptr)
	{
		Result = EBTNodeResult::Failed;
	}

	UAnimInstance* AnimInstance = Cast<ACharacter>(AIController->GetPawn())->GetMesh()->GetAnimInstance();
	if(AnimInstance == nullptr)
	{
		Result = EBTNodeResult::Failed;
	}

	int8 RandomNumber = UKismetMathLibrary::RandomInteger(AnimMontages.Num());
	UAnimMontage* AnimMontage = AnimMontages[RandomNumber];
	AnimInstance->Montage_Play(AnimMontage);

	FOnMontageBlendingOutStarted BlendOutDelegate;
	BlendOutDelegate.BindUObject(this, &ThisClass::ExecuteOnMontageEnded);
	AnimInstance->Montage_SetBlendingOutDelegate(BlendOutDelegate, AnimMontage);

	FOnMontageEnded EndedDelegate;
	BlendOutDelegate.BindUObject(this, &ThisClass::ExecuteOnMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndedDelegate, AnimMontage);
	
	return Result;
}

void UBTT_PlayRandomAnimMontage::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	SetNextTickTime(NodeMemory, 0.5f);

	if(IsMontageEnded)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTT_PlayRandomAnimMontage::ExecuteOnMontageEnded(UAnimMontage* animMontage, bool bInterrupted)
{
	IsMontageEnded = true;
}
