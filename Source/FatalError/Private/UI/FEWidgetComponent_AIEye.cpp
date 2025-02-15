// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/FEWidgetComponent_AIEye.h"
#include "AI/FEAIDetectionInfoInterface.h"
#include "UI/FEAIWidget_AIEye.h"
#include "AI/FEAIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/ProgressBar.h"
#include "Util/ColorConstants.h"

UFEWidgetComponent_AIEye::UFEWidgetComponent_AIEye()
{
	bIsBegin = false;
	SightValuePercent = 0.0f;
	PredictedDetectStateColor = YellowColor;
	bCanChangeColor = true;
	DescendingWaitTime = 10.0f;
}

void UFEWidgetComponent_AIEye::BeginPlay()
{
	Super::BeginPlay();
	PrimaryComponentTick.bCanEverTick = true;
	
	bIsBegin = true;
	EyeIconWidget = Cast<UFEAIWidget_AIEye>(GetUserWidgetObject());
	AFEAIController* AIController = Cast<AFEAIController>(UAIBlueprintHelperLibrary::GetAIController(GetOwner()));
	if(AIController != nullptr)
	{
		AIDetectionInfo = Cast<IFEAIDetectionInfoInterface>(AIController);
	}
}

void UFEWidgetComponent_AIEye::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bIsBegin && IsValid(EyeIconWidget) && AIDetectionInfo != nullptr)
	{
		SetCurrentDetectState();
		SetSightStrengthMultiplier();
	}
}

void UFEWidgetComponent_AIEye::SetCurrentDetectState()
{
	// 감지 여부에 따라 게이지 퍼센트 상승/하강
	float InterpTarget = 0.0f;
	float InterpSpeed = 0.0f;

	if(AIDetectionInfo->GetIsTargetDetected())
	{
		InterpTarget = 1.0f;
		InterpSpeed = AIDetectionInfo->GetTargetSightStrength();
	}
	else if(AIDetectionInfo->GetIsHearingSound())
	{
		InterpTarget = 1.0f;
		InterpSpeed = AIDetectionInfo->GetTargetHearingStrength();
	}
	else if(bGageDescendingStop)
	{
		InterpSpeed = 0.0f; 
	}
	else
	{
		InterpTarget = 0.0f;
		InterpSpeed = 0.1f;
	}  
	SightValuePercent = FMath::FInterpConstantTo(SightValuePercent, InterpTarget, GetWorld()->GetDeltaSeconds(), InterpSpeed);
	EyeIconWidget->DetectionGage->SetPercent(SightValuePercent);
	AIDetectionInfo->SetIsPercentFull(SightValuePercent == 1.0f ? true : false);

	// 게이지가 상승하여 게이지가 가득 찼을 때, 예상 단계로 전환
	if(AIDetectionInfo->GetIsPercentFull() && bCanChangeColor && !bGageDescendingStop)
	{
		AIDetectionInfo->SetCurrentDetectState(AIDetectionInfo->GetPredictedDetectState());
		SetFillColor();
		bCanChangeColor = false;
	}
	
	// 특정 감지 단계에서 게이지 퍼센트가 0이 될 때 이전 단계 색상으로 전환
	if(EyeIconWidget->DetectionGage->GetPercent() == 0.0f)
	{
		switch (AIDetectionInfo->GetCurrentDetectState())
		{
		case EFEDetectState::Red:
			AIDetectionInfo->SetIsTargetDetected(true);
			SightValuePercent = 1.0f;
			AIDetectionInfo->SetCurrentDetectState(EFEDetectState::Orange);
			SetFillColor();
			EyeIconWidget->DetectionGage->SetPercent(SightValuePercent);
			break;
		case EFEDetectState::Orange:
			AIDetectionInfo->SetIsTargetDetected(true);
			SightValuePercent = 1.0f;
			AIDetectionInfo->SetCurrentDetectState(EFEDetectState::Yellow);
			SetFillColor();
			EyeIconWidget->DetectionGage->SetPercent(SightValuePercent);
			break;
		case EFEDetectState::Yellow:
			AIDetectionInfo->SetCurrentDetectState(EFEDetectState::Green);
			SetFillColor();
			bCanChangeColor = true;
			break;
		}
	}
	
	// 일정 시야 감도 이상이면 바로 특정 단계로 변경
	if(AIDetectionInfo->GetTargetSightStrength() >= 0.9f)
	{
		SightValuePercent = 1.0f;
		EyeIconWidget->DetectionGage->SetPercent(SightValuePercent);
		AIDetectionInfo->SetIsPercentFull(true);
		AIDetectionInfo->SetCurrentDetectState(EFEDetectState::Red);
		SetFillColor();
	}
	else if(AIDetectionInfo->GetTargetSightStrength() >= 0.7f)
	{
		if(AIDetectionInfo->GetCurrentDetectState() <= EFEDetectState::Yellow)
		{
			SightValuePercent = 1.0f;
			EyeIconWidget->DetectionGage->SetPercent(SightValuePercent);
			AIDetectionInfo->SetIsPercentFull(true);
			AIDetectionInfo->SetCurrentDetectState(EFEDetectState::Orange);
			SetFillColor();
		}
	}

	// 일정 청각 감도 이상이면 바로 주황색 단계로 변경
	if(AIDetectionInfo->GetTargetHearingStrength() >= 0.7f)
	{
		if(AIDetectionInfo->GetCurrentDetectState() <= EFEDetectState::Yellow)
		{
			SightValuePercent = 1.0f;
			EyeIconWidget->DetectionGage->SetPercent(SightValuePercent);
			AIDetectionInfo->SetIsPercentFull(true);
			AIDetectionInfo->SetCurrentDetectState(EFEDetectState::Orange);
			SetFillColor();
		}
	}
}

// 현재 감지 단계에 맞게 위젯 색상 변경
void UFEWidgetComponent_AIEye::SetFillColor()
{
	switch (AIDetectionInfo->GetCurrentDetectState())
	{
	case EFEDetectState::Green:
		EyeIconWidget->DetectionGage->SetFillColorAndOpacity(GreenColor);
		break;
	case EFEDetectState::Yellow: 
		EyeIconWidget->DetectionGage->SetFillColorAndOpacity(YellowColor);
		break;
	case EFEDetectState::Orange:
		EyeIconWidget->DetectionGage->SetFillColorAndOpacity(OrangeColor);
		break;
	case EFEDetectState::Red:
		EyeIconWidget->DetectionGage->SetFillColorAndOpacity(RedColor);
		break;
	}
}

// 감지 단계가 올라갈수록 감지 강도 증가
void UFEWidgetComponent_AIEye::SetSightStrengthMultiplier()
{
	if(AIDetectionInfo->GetCurrentDetectState() == EFEDetectState::Green)
	{
		AIDetectionInfo->SetSightStrengthMultiplier(0.25f);
	}
	else if(AIDetectionInfo->GetCurrentDetectState() == EFEDetectState::Yellow)
	{
		AIDetectionInfo->SetSightStrengthMultiplier(0.3f);
	}
	else
	{
		AIDetectionInfo->SetSightStrengthMultiplier(0.35f);
	}
}