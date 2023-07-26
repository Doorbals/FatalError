// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FEI_CarryBody.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFEI_CarryBody : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FATALERROR_API IFEI_CarryBody
{
	GENERATED_BODY()

public:
	virtual void CarriedByPlayer(FVector& Location, FRotator& Rotation) = 0;

	virtual void DroppedByPlayer(FVector& Location, FRotator& Rotation) = 0;
};
