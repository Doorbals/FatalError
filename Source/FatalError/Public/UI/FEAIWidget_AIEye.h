// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FEAIWidget_AIEye.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UFEAIWidget_AIEye : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY()
	TObjectPtr<class UProgressBar> DetectionGage;
};
