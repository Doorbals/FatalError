// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "FatalError/FatalError.h"
#include "FESpiderRobotPawn.generated.h"

DECLARE_DELEGATE_OneParam(FOnRemainingTimeChanged, const float);
DECLARE_MULTICAST_DELEGATE(FOnPossessedSpiderRobot);
DECLARE_MULTICAST_DELEGATE(FOnUnPossessedSpiderRobot);

UCLASS()
class FATALERROR_API AFESpiderRobotPawn : public APawn
{
	GENERATED_BODY()

public:
	AFESpiderRobotPawn();

	virtual void PossessedBy(AController* NewController) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void TraceSurface(FVector2d MovementVector);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UBoxComponent* RootBox;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class USkeletalMeshComponent* Mesh;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UCameraComponent* FollowCamera;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UBoxComponent* PossessArea;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UFloatingPawnMovement* FloatingPawnMovement;

	bool bIsTerminated;

	bool bIsPossessed;

	float CurrentRemainingTime;

	float BaseRemainingTime;

	ACharacter* OriginPlayerCharacter;

	FVector SurfaceLocation;

	FVector TargetLocation;

	FRotator TargetRotation;

	class AFEPlayerController* PlayerController;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* TerminateAction;

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void Terminate();

	UPROPERTY(BlueprintReadWrite)
	EFEMovementState MovementState;

	FOnRemainingTimeChanged RemainingTimeChangedDelegate;
	FOnPossessedSpiderRobot PossessedSpiderRobot;
	FOnUnPossessedSpiderRobot UnPossessedSpiderRobot;
};
