// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_PlayRandomAnimMontage.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UBTT_PlayRandomAnimMontage : public UBTTaskNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Condition)
	TArray<UAnimMontage*> AnimMontages;

	bool IsMontageStarted;
	
	bool IsMontageEnded;
	
protected:
	UBTT_PlayRandomAnimMontage();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	void ExecuteOnMontageEnded(UAnimMontage* animMontage, bool bInterrupted);
};
