// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FEAICharacterBase.h"
#include "AI/FEAIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

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
	AFEAIController* AIController = Cast<AFEAIController>(AIControllerClass);
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

