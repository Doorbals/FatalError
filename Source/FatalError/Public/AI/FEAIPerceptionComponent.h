// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "FEAIPerceptionComponent.generated.h"

/**
 * 
 */

// 자극이 완전히 사라질 때 해당 자극에 대한 정보 전달하기 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExpiredStimulus, FAIStimulus&, StimulusStore);

UCLASS()
class FATALERROR_API UFEAIPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()
public:
	virtual void HandleExpiredStimulus(FAIStimulus& StimulusStore) override;

	FExpiredStimulus OnStimulusExpired;
};
