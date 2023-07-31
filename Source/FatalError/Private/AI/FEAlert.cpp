// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FEAlert.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFEAlert::AFEAlert()
{
 	AlertCoolTime = 60.0f;
	IsPushedAlert = false;
}

void AFEAlert::PushAlert_Implementation()
{
	IsPushedAlert = true;
	GetWorld()->GetTimerManager().SetTimer(AlertTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				GetWorld()->GetTimerManager().ClearTimer(AlertTimerHandle);
				FinishCoolTime();
			}), AlertCoolTime, false);
	AlertConditionOnChangedDelegate.Broadcast(IsPushedAlert);
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	
	UGameplayStatics::ApplyRadialDamage(GetWorld(), 0.0f, GetActorLocation(), 2000.0f,
		nullptr, TArray<AActor*>(), 0, 0, false, ECC_Visibility);
}

void AFEAlert::FinishCoolTime()
{
	IsPushedAlert = false;
	AlertConditionOnChangedDelegate.Broadcast(IsPushedAlert);
}

