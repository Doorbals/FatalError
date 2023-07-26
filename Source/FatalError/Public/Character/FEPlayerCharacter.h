// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character/FECharacterBase.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
#include "FatalError/FatalError.h"
#include "Perception/AISightTargetInterface.h"
#include "FEPlayerCharacter.generated.h"

/**
 * 
 */

// FEAIController 클래스에서 타겟의 Location을 사용하기 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAIPerceptionTargetOnUpdated, const FVector&, TargetLocation);

UCLASS()
class FATALERROR_API AFEPlayerCharacter : public AFECharacterBase, public IAISightTargetInterface
{
	GENERATED_BODY()

public:
	AFEPlayerCharacter(const class FObjectInitializer& ObjectInitializer);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible, int32* UserData) override;
	
	class USpringArmComponent* GetCameraBoom();

	class UCameraComponent* GetFollowCamera();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float BaseTurnRate = 45.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
	float BaseLookUpRate = 45.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GASDocumentation|Camera")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GASDocumentation|Camera")
	class UCameraComponent* FollowCamera;

	float BaseCapsuleHalfHeight = 88.0f;
	
	FGameplayTag DeadTag;

	APlayerCameraManager* CameraManager;
	
	virtual void BeginPlay() override;
	
	void InitControlMode();
	
	void Move(const FInputActionValue& Value);

	void SneakMove(const FInputActionValue& Value);

	void ClimbMove(const FInputActionValue& Value);
	
	void StopMove(const FInputActionValue& Value);
	
	void Look(const FInputActionValue& Value);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);

	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UFEInputConfig* InputConfig;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* SneakInputMapping;

	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputMappingContext* ClimbInputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* AssassinateMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TArray<class UAnimMontage*> AttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* CarryBodyMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* DropBodyMontage;
		
	UFUNCTION(BlueprintCallable)
	void SetControlMode(EFEMovementState InMovementState);
	
	virtual void Jump() override;
	
	virtual void StopJumping() override;
	
	void ToggleCrouch();

	void WallTrace();

	void StartSneak(FVector PlaneNormal);

	void EndSneak();

	void SneakMoveTrace(float InMoveRight, float InMoveForward);
	
	void CheckCameraXAxisLocation();

	void CheckCameraYAxisLocation();

	uint8 CameraXAxisLocation;

	uint8 CameraYAxisLocation;

	UPROPERTY(BlueprintReadOnly)
	bool bIsSneak;

	UPROPERTY(BlueprintReadOnly)
	bool bSneakRight;

	UPROPERTY(BlueprintReadWrite)
	EFEMovementState MovementState;

	UPROPERTY(BlueprintReadWrite)
	float MovingLeftRight;
	
	UPROPERTY(BlueprintReadWrite)
	bool bCanMoveRight;
	
	UPROPERTY(BlueprintReadWrite)
	bool bCanMoveLeft;
	
	UPROPERTY(BlueprintReadWrite)
	FVector WorldDirection;

	UPROPERTY(BlueprintReadWrite)
	bool bMovementEnabled;

	UPROPERTY(BlueprintReadWrite)
	bool bIsCarryingBody;

	uint8 AttackMontagesOrder;
	
	FAIPerceptionTargetOnUpdated TargetOnUpdated;

	// SpringArm Timeline
	UPROPERTY(EditAnywhere, Category="Timeline")
	UCurveFloat* SpringArmTimelineCurve;
	
	UTimelineComponent* SpringArmTimeline;

	UTimelineComponent* ClimbOuterTurnTimeline;
	
private:
	FOnTimelineFloat SpringArmTimelineUpdateDelegate;

	FOnTimelineFloat ClimbOuterTurnTimelineUpdateDelegate;

	FOnTimelineEvent ClimbOuterTurnTimelineFinishDelegate;

	UFUNCTION()
	void SpringArmTimelineUpdateFunc(float Output);
	
	UFUNCTION()
	void ClimbOuterTurnTimelineUpdateFunc(float Output);

	UFUNCTION()
	void ClimbOuterTurnTimelineFinishFunc();
};
