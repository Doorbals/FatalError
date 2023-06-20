// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FEAIWidget_AIEye.h"

#include "Components/ProgressBar.h"

void UFEAIWidget_AIEye::NativeConstruct()
{
	Super::NativeConstruct();
	DetectionGage = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_DetectionGage")));

	if(IsValid(DetectionGage))
	{
		DetectionGage->SetFillColorAndOpacity(FLinearColor(0.036149f, 0.536458f, 0.0f));
	}
}
