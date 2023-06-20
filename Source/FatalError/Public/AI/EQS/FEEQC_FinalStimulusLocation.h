// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "FEEQC_FinalStimulusLocation.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UFEEQC_FinalStimulusLocation : public UEnvQueryContext
{
	GENERATED_BODY()

public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
