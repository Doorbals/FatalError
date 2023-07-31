// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FEHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UFEHUDWidget : public UUserWidget
{
	GENERATED_BODY()

	UFEHUDWidget(const FObjectInitializer& ObjectInitializer);
	
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void UpdateHp(float NewPercent);

	void UpdateEnergy(float NewPercent);

	void UpdateSpiderRobotRemainingTime(float InTime);

	void PlayUpAnimation();

	void PlayDownAnimation();
	
	UPROPERTY()
	TObjectPtr<class UProgressBar> HealthProgressBar;
	
	UPROPERTY()
	TObjectPtr<class UProgressBar> EnergyProgressBar;

	UPROPERTY()
	TObjectPtr<class UGridPanel> TimeGridPanel;

	UPROPERTY()
	TObjectPtr<class UTextBlock> TimeText;
	
	UPROPERTY(BlueprintReadWrite)
	class UWidgetAnimation* DownAnimation;

	UPROPERTY(BlueprintReadWrite)
	class UWidgetAnimation* UpAnimation;

protected:
	float NewHealthPercent;

	float NewEnergyPercent;
};

