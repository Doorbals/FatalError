// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character/FECharacterBase.h"
#include "Input/FEInputConfig.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "FatalError/FatalError.h"
#include "FEPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API AFEPlayerCharacter : public AFECharacterBase
{
	GENERATED_BODY()

public:
	AFEPlayerCharacter(const class FObjectInitializer& ObjectInitializer);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	class USpringArmComponent* GetCameraBoom();

	class UCameraComponent* GetFollowCamera();

	UFUNCTION(BlueprintCallable, Category = "Camera")
	float GetStartingCameraBoomArmLength();

	UFUNCTION(BlueprintCallable, Category = "Camera")
	FVector GetStartingCameraBoomLocation();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float BaseTurnRate = 45.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float BaseLookUpRate = 45.0f;

	UPROPERTY(BlueprintReadOnly, Category = "GASDocumentation|Camera")
	float StartingCameraBoomArmLength;

	UPROPERTY(BlueprintReadOnly, Category = "GASDocumentation|Camera")
	FVector StartingCameraBoomLocation;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GASDocumentation|Camera")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GASDocumentation|Camera")
	class UCameraComponent* FollowCamera;

	bool ASCInputBound = false;

	FGameplayTag DeadTag;
	
	virtual void BeginPlay() override;
	
	void InitControlMode();
	
	void Move(const FInputActionValue& Value);

	void StopMove(const FInputActionValue& Value);
	
	void Look(const FInputActionValue& Value);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);

	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
	
	virtual void OnRep_PlayerState() override;

	// 잠재적인 race Condition을 처리하기 위해 SetupPlayerInputComponent와 OnRep_PlayerState 양쪽에서 모두 호출된다. because of a potential race condition where the PlayerController might
	// PlayerController가 ClientRestart를 호출하고, 이로 인해 PlayerState가 클라이언트로 리플리케이트 되기 전에 SetupPlayerInputComponent가 호출될 수 있으므로, SetupPlayerInputComponent에서 PlayerState가 null이 될 수 있다.
	// 반대로, PlayerController가 ClientRestart를 호출하기 전에 PlayerState가 리플리케이트 되는 경우, OnRep_PlayerState에서 Actor의 InputComponent가 null이 될 수 있다. (즉, InputComponent가 PlayerController에서 설정되기 전에 PlayerState가 설정되는 경우)
	void BindASCInput();
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UFEInputConfig* InputConfig;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* InputMapping;

	UFUNCTION(BlueprintCallable)
	void SetControlMode(EFEMovementState InMovementState);
	
	virtual void Jump() override;
	
	virtual void StopJumping() override;

	void ToggleCrouch();

	void WallTrace();

	void StartSneak(FVector PlaneNormal);

	void EndSneak();

	void SneakTrace(float InMoveRight, float InMoveForward);

	UPROPERTY(BlueprintReadOnly)
	bool bIsSneak = false;

	UPROPERTY(BlueprintReadOnly)
	bool bSneakRight = true;

	UPROPERTY(BlueprintReadWrite)
	EFEMovementState MovementState;

	UPROPERTY(BlueprintReadWrite)
	float MovingLeftRight;

	UPROPERTY(BlueprintReadWrite)
	float MovingUpDown;

	UPROPERTY(BlueprintReadWrite)
	bool bCanMoveRight;
	
	UPROPERTY(BlueprintReadWrite)
	bool bCanMoveLeft;
	
	UPROPERTY(BlueprintReadWrite)
	FVector WorldDirection;
};
