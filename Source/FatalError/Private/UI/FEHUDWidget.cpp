// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FEHUDWidget.h"
#include "MovieScene.h"
#include "Animation/WidgetAnimation.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"


UFEHUDWidget::UFEHUDWidget(const FObjectInitializer& ObjectInitializer):UUserWidget(ObjectInitializer)
{
}

void UFEHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HealthProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HpBar")));
	EnergyProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("EnergyBar")));
	TimeText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Time")));
	DownAnimation = GetWidgetTreeOwningClass()->Animations[0];
	UpAnimation = GetWidgetTreeOwningClass()->Animations[1];
}

void UFEHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(HealthProgressBar->GetPercent() > NewHealthPercent)
	{
		HealthProgressBar->SetPercent(FMath::FInterpTo(HealthProgressBar->GetPercent(), NewHealthPercent, InDeltaTime, 10.0f));
	}

	if(EnergyProgressBar->GetPercent() > NewEnergyPercent)
	{
		EnergyProgressBar->SetPercent(FMath::FInterpTo(EnergyProgressBar->GetPercent(), NewEnergyPercent, InDeltaTime, 10.0f));
	}
}

void UFEHUDWidget::UpdateHp(float NewPercent)
{
	NewHealthPercent = NewPercent;
}

void UFEHUDWidget::UpdateEnergy(float NewPercent)
{
	NewEnergyPercent = NewPercent;
}

void UFEHUDWidget::UpdateSpiderRobotRemainingTime(float InTime)
{
	TimeText->SetText(UKismetTextLibrary::Conv_FloatToText(InTime, ERoundingMode::HalfToEven, false, true,
		2, 324, 2, 2));
}

