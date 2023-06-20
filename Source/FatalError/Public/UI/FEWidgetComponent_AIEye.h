// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "FEWidgetComponent_AIEye.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UFEWidgetComponent_AIEye : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UFEWidgetComponent_AIEye();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetCurrentDetectState();

	void SetFillColor();

	void SetSightStrengthMultiplier();
	
	UPROPERTY()
	TObjectPtr<class UFEAIWidget_AIEye> EyeIconWidget;
	
	class IFEAIDetectionInfoInterface* AIDetectionInfo;
	
	float SightValuePercent;

	bool bIsBegin;

	bool bCanChangeColor;
	
	UPROPERTY(BlueprintReadOnly)
	bool bGageDescendingStop;

	float DescendingWaitTime;
	
	FTimerHandle GageDescendingTimerHandle;

	FLinearColor PredictedDetectStateColor;
	
	FLinearColor RedColor = FLinearColor(0.618055f, 0.009891f, 0.0f);
	FLinearColor OrangeColor = FLinearColor(1.0f, 0.238456f, 0.0f);
	FLinearColor YellowColor = FLinearColor(1.0f, 0.852586f, 0.0f);
	FLinearColor GreenColor = FLinearColor(0.036149f, 0.536458f, 0.0f);
};
