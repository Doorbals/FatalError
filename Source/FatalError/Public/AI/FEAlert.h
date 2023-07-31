// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FEAlert.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FAlertConditionOnChanged, bool);

UCLASS()
class FATALERROR_API AFEAlert : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFEAlert();

	FAlertConditionOnChanged AlertConditionOnChangedDelegate;

	UFUNCTION(BlueprintNativeEvent)
	void PushAlert();
	virtual void PushAlert_Implementation();

protected:
	float AlertCoolTime;

	bool IsPushedAlert;

	FTimerHandle AlertTimerHandle;

	void FinishCoolTime();
};
