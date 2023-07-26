// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Character/Abilities/AbilityInterface/FEI_Assassination.h"
#include "Character/Abilities/AbilityInterface/FEI_CarryBody.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "GameFramework/Actor.h"
#include "FEAICharacterBase.generated.h"

UCLASS()
class FATALERROR_API AFEAICharacterBase : public ACharacter, public IFEI_Assassination, public IFEI_CarryBody, public IAISightTargetInterface
{
	GENERATED_BODY()

public:
	AFEAICharacterBase();

	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible, int32* UserData) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void Assassinated(FVector& Location, FRotator& Rotation) override;

	UFUNCTION(BlueprintCallable)
	virtual void Attacked(FVector& Location, FRotator& Rotation) override;

	UFUNCTION(BlueprintCallable)
	virtual void CarriedByPlayer(FVector& Location, FRotator& Rotation) override;

	UFUNCTION(BlueprintCallable)
	virtual void DroppedByPlayer(FVector& Location, FRotator& Rotation) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	class UFEWidgetComponent_AIEye* EyeWidget;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	class UAnimMontage* AssassinatedMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	TArray<class UAnimMontage*> AttackedMontages;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	class UAnimMontage* CarriedBodyMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	class UAnimMontage* DroppedBodyMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsDeadBodyDetected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsDetectedInRedState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsCarried;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsRagDoll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsRecovering;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsSimulating;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float PhysicsAlpha;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
	
	void ExecuteOnAssassinateMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted);

	void ExecuteOnCarryBodyMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted);
	
	void ExecuteOnDropBodyMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UBoxComponent> CarryBodyArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> AssassinRep;

	TObjectPtr<class USkeletalMeshComponent> SkeletalMesh;
	
	float CarryBodyXOffset;

	float CarryBodyYOffset;

	float AttackXOffset;

	float AttackYOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FName PhysicsBoneName;

	class IFEAIDetectionInfoInterface* AIDetectionInfo;
};
