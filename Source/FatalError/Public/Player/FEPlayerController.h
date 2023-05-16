// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Character/FECharacterBase.h"
#include "FEPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API AFEPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:

	// Server only
	virtual void OnPossess(APawn* InPawn) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	UFEAbilitySystemComponent* GetFEAbilitySystemComponent();
};
