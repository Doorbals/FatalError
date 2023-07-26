// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Task/BTT_PlayAnimMontage.h"

#include "AIController.h"
#include "Animation/FEAIAnimInstance.h"
#include "GameFramework/Character.h"

UBTT_PlayAnimMontage::UBTT_PlayAnimMontage()
{
	NodeName = FString("Play Anim Montage");
	bTickIntervals = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_PlayAnimMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	AnimInstance->Montage_Play(AnimMontage);

	FOnMontageBlendingOutStarted BlendOutDelegate;
	BlendOutDelegate.BindUObject(this, &ThisClass::ExecuteOnMontageEnded);
	AnimInstance->Montage_SetBlendingOutDelegate(BlendOutDelegate, AnimMontage);

	FOnMontageEnded EndedDelegate;
	BlendOutDelegate.BindUObject(this, &ThisClass::ExecuteOnMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndedDelegate, AnimMontage);
	
	return Result;
}

void UBTT_PlayAnimMontage::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	SetNextTickTime(NodeMemory, 0.5f);

	if(IsMontageEnded)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTT_PlayAnimMontage::ExecuteOnMontageEnded(UAnimMontage* animMontage, bool bInterrupted)
{
	IsMontageEnded = true;
}
