// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "Character/FECharacterBase.h"
#include "FEPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API AFEPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	AFEPlayerController();
	
	FGenericTeamId TeamId;
	
	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual void SetGenericTeamId(const FGenericTeamId& InTeamId) override { TeamId = InTeamId; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=HUD)
	TSubclassOf<class UFEHUDWidget> FEHUDWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=HUD)
	TObjectPtr<class UFEHUDWidget> FEHUDWidget;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void OnPossess(APawn* InPawn) override;
	
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	
	UFEAbilitySystemComponent* GetFEAbilitySystemComponent();

	
	
};
