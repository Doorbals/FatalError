// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/FEAICharacterBase.h"
#include "AI/FEAIController.h"

// Sets default values
AFEAICharacterBase::AFEAICharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = AFEAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AFEAICharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFEAICharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFEAICharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AFEAICharacterBase::GetAIPatrolRadius()
{
	return 800.0f;
}

float AFEAICharacterBase::GetAIDecentRange()
{
	return 400.0f;
}

float AFEAICharacterBase::GetAIAttackRange()
{
	return 100.0f;
}

float AFEAICharacterBase::GetAITurnSpeed()
{
	return 0.0f;
}

void AFEAICharacterBase::SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
	OnAttackFinished = InOnAttackFinished;
}

void AFEAICharacterBase::AttackByAI()
{
	UE_LOG(LogTemp, Log, TEXT("Attack"));
	NotifyComboActionEnd();
}

void AFEAICharacterBase::NotifyComboActionEnd()
{
	UE_LOG(LogTemp, Log, TEXT("Attack End"));
	OnAttackFinished.ExecuteIfBound();
}



