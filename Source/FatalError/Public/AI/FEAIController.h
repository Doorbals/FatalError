  // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FatalError.h"
#include "FEAIDetectionInfoInterface.h"
#include "FEAIController.generated.h"

/**
 * 
 */

UCLASS()
class FATALERROR_API AFEAIController : public AAIController, public IFEAIDetectionInfoInterface
{
	GENERATED_BODY()

public:
	AFEAIController();
	
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	
	virtual void BeginPlay() override;
	

	UFUNCTION()
	void OnTargetPerceptionUpdated_Delegate(AActor* Actor, struct FAIStimulus Stimulus);

	UFUNCTION()
	void OnStimulusExpired_Delegate(FAIStimulus& StimulusStore);

	UFUNCTION()
	void CalculateSightStrength_Delegate(const FVector& TargetLocation);

	UFUNCTION()
	void CalculateHearingStrength(const FVector& TargetLocation);
	
	void RunAI();
	
	void StopAI();

protected:
	virtual void OnPossess(APawn* InPawn) override;

public:
	UPROPERTY()
	TObjectPtr<class UBlackboardData> BBAsset;

	UPROPERTY()
	TObjectPtr<class UBehaviorTree> BTAsset;

	UPROPERTY(VisibleAnywhere, Category=AI)
	TObjectPtr<class UFEAIPerceptionComponent> AIPerceptionComponent;
	TObjectPtr<class UAISenseConfig_Sight> AISenseConfigSight;
	TObjectPtr<class UAISenseConfig_Hearing> AISenseConfigHearing;
	TObjectPtr<class UAISenseConfig_Damage> AISenseConfigDamage;
	
	UPROPERTY(BlueprintReadWrite)
	EFEDetectState CurrentDetectState;

	UPROPERTY(BlueprintReadWrite)
	EFEDetectState PredictedDetectState;

	UPROPERTY(BlueprintReadWrite)
	float SightStrengthMultiplier;

	UPROPERTY(BlueprintReadWrite)
	bool bIsPercentFull;
	
	UPROPERTY(BlueprintReadWrite)
	float TargetSightStrength;

	UPROPERTY(BlueprintReadWrite)
	bool bIsTargetDetected;

	UPROPERTY(BlueprintReadWrite)
	float TargetHearingStrength;
	
	UPROPERTY(BlueprintReadWrite)
	bool bIsHearingSound;

	UPROPERTY(BlueprintReadWrite)
	FVector FinalStimulusLocation;

	UPROPERTY(BlueprintReadWrite)
	bool bIsTargetOutOfSight;

protected:
	virtual EFEDetectState GetCurrentDetectState() override { return CurrentDetectState; }
	virtual void SetCurrentDetectState(EFEDetectState InCurrentDetectState) override;

	virtual EFEDetectState GetPredictedDetectState() override { return PredictedDetectState; }
	virtual void SetPredictedDetectState(EFEDetectState InPredictedDetectState) override { PredictedDetectState = InPredictedDetectState; }

	virtual float GetSightStrengthMultiplier() override { return SightStrengthMultiplier; }
	virtual void SetSightStrengthMultiplier(float InValue) override { SightStrengthMultiplier = InValue; }
	
	virtual bool GetIsPercentFull() override { return bIsPercentFull; }
	virtual void SetIsPercentFull(bool InValue) override { bIsPercentFull = InValue; }

	virtual float GetTargetSightStrength() override { return TargetSightStrength; }
	virtual void SetTargetSightStrength(float InValue) override { TargetSightStrength = InValue; }

	virtual bool GetIsTargetDetected() override { return bIsTargetDetected; }
	virtual void SetIsTargetDetected(bool InValue) override { bIsTargetDetected = InValue; }

	virtual float GetTargetHearingStrength() override { return TargetHearingStrength; }
	virtual void SetTargetHearingStrength(float InValue) override { TargetHearingStrength = InValue; }

	virtual bool GetIsHearingSound() override { return bIsHearingSound; }
	virtual void SetIsHearingSound(bool InValue) override { bIsHearingSound = InValue; }

	virtual FVector GetFinalStimulusLocation() override { return FinalStimulusLocation; }
	virtual void SetFinalStimulusLocation(const FVector& InValue) override { FinalStimulusLocation = InValue; }

	virtual bool GetIsTargetOutOfSight() override { return bIsTargetOutOfSight; } 
	virtual void SetIsTargetOutOfSight(bool InValue) override { bIsTargetOutOfSight = InValue; }
};


