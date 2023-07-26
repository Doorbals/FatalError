// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FatalError.h"
#include "FEAIDetectionInfoInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFEAIDetectionInfoInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */

// AI가 적을 Detect 하는 데에 사용하는 변수들에 대한 Get, Set 함수들 모아놓은 인터페이스
class FATALERROR_API IFEAIDetectionInfoInterface
{
	GENERATED_BODY()
	
public:
	virtual EFEDetectState GetCurrentDetectState() = 0;
	virtual void SetCurrentDetectState(EFEDetectState InCurrentDetectState) = 0;

	virtual EFEDetectState GetPredictedDetectState() = 0;
	virtual void SetPredictedDetectState(EFEDetectState InPredictedDetectState) = 0;

	virtual float GetSightStrengthMultiplier() = 0;
	virtual void SetSightStrengthMultiplier(float InValue) = 0;
	
	virtual bool GetIsPercentFull() = 0;
	virtual void SetIsPercentFull(bool InValue) = 0;

	virtual float GetTargetSightStrength() = 0;
	virtual void SetTargetSightStrength(float InValue) = 0;

	virtual bool GetIsTargetDetected() = 0;
	virtual void SetIsTargetDetected(bool InValue) = 0;

	virtual float GetTargetHearingStrength() = 0;
	virtual void SetTargetHearingStrength(float InValue) = 0;

	virtual bool GetIsHearingSound() = 0;
	virtual void SetIsHearingSound(bool InValue) = 0;

	virtual FVector GetFinalStimulusLocation() = 0;
	virtual void SetFinalStimulusLocation(const FVector& InValue) = 0;

	virtual bool GetIsTargetOutOfSight() = 0;
	virtual void SetIsTargetOutOfSight(bool InValue) = 0;
};
