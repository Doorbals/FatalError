// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FESpiderRobotPawn.h"

#include "AIController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/FEPlayerController.h"
#include "UI/FEHUDWidget.h"

// Sets default values
AFESpiderRobotPawn::AFESpiderRobotPawn()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootBox = CreateDefaultSubobject<UBoxComponent>(FName("Box"));
	
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName("SkeletalMesh"));
	Mesh->SetupAttachment(RootBox);
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
	CameraBoom->SetupAttachment(RootBox);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(FName("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);

	PossessArea = CreateDefaultSubobject<UBoxComponent>(FName("PossessArea"));
	PossessArea->SetupAttachment(RootBox);

	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(FName("FloatingPawnMovement"));
	
	BaseRemainingTime = 40.0f;
}

void AFESpiderRobotPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PlayerController = Cast<AFEPlayerController>(NewController);
	if(PlayerController != nullptr)
	{
		bIsPossessed = true;
		CurrentRemainingTime = BaseRemainingTime;
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(InputMapping, 0);
		PlayerController->SetGenericTeamId(FGenericTeamId(1));
		PlayerController->FEHUDWidget->PlayAnimation(PlayerController->FEHUDWidget->DownAnimation);
	}
}

// Called when the game starts or when spawned
void AFESpiderRobotPawn::BeginPlay()
{
	Super::BeginPlay();

	OriginPlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	TargetLocation = GetActorLocation();
	PlayerController = Cast<AFEPlayerController>(GetController());
}

// Called every frame
void AFESpiderRobotPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!bIsTerminated && bIsPossessed)
	{
		CurrentRemainingTime = FMath::Clamp(CurrentRemainingTime - DeltaTime, 0.0f, BaseRemainingTime);
		PlayerController->FEHUDWidget->UpdateSpiderRobotRemainingTime(CurrentRemainingTime);
		if(CurrentRemainingTime <= 0.0f)
		{
			Terminate();
		}
	}
}

// Called to bind functionality to input
void AFESpiderRobotPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFESpiderRobotPawn::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFESpiderRobotPawn::Look);
		EnhancedInputComponent->BindAction(TerminateAction, ETriggerEvent::Triggered, this, &AFESpiderRobotPawn::Terminate);
	}
}

void AFESpiderRobotPawn::TraceSurface(FVector2d MovementVector)
{
	FHitResult InnerHitResult1, InnerHitResult2, InnerHitResultR, OuterHitResult1, OuterHitResult2, OuterHitResultR;
	FCollisionQueryParams CollisionQueryParams;
	FVector MovementDirection = GetActorForwardVector() * MovementVector.Y + GetActorRightVector() * MovementVector.X;
	MovementDirection.Normalize();

	// 첫 번째 트레이스
	FVector StartLocation = GetActorLocation() + GetActorUpVector() * 50.0f;
	FVector EndLocation = MovementDirection * 100.0f + GetActorUpVector() * -50.0f;
	EndLocation.Normalize();
	EndLocation = GetActorLocation() + EndLocation * 200.0f; 
	GetWorld()->LineTraceSingleByChannel(InnerHitResult1, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility);
	//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor(255, 0, 0), false, 0.5f, 0,0.5f);

	if(InnerHitResult1.bBlockingHit)
	{
		// 두 번째 트레이스
		EndLocation = MovementDirection * 110.0f + GetActorUpVector() * -50.0f;
		EndLocation.Normalize();
		EndLocation = GetActorLocation() + EndLocation * 200.0f;
		GetWorld()->LineTraceSingleByChannel(InnerHitResult2, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility);
		//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor(255, 255, 255), false, 0.5f, 0,0.5f);
		if(InnerHitResult2.bBlockingHit)
		{
			// 세 번째 트레이스
			FVector RotatedVector = UKismetMathLibrary::RotateAngleAxis(MovementDirection, 90.0f, GetActorUpVector());
			EndLocation = GetActorLocation() + MovementDirection * 110.0f + GetActorUpVector() * -50.0f + RotatedVector * 5.0f;
			GetWorld()->LineTraceSingleByChannel(InnerHitResultR, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility);
			//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor(0, 255, 0), false, 0.5f, 0,0.5f);
			if(InnerHitResultR.bBlockingHit)
			{
				TargetLocation = InnerHitResult1.Location;
				TargetRotation = UKismetMathLibrary::MakeRotationFromAxes(
					UKismetMathLibrary::GetDirectionUnitVector(InnerHitResult1.Location, InnerHitResult2.Location),
					UKismetMathLibrary::GetDirectionUnitVector(InnerHitResult1.Location, InnerHitResultR.Location),
					InnerHitResult1.Normal);
			}
		}
	}
	else
	{
		MovementDirection = MovementDirection * 500.0f + GetActorUpVector() * -300.0f;
		MovementDirection.Normalize();
		StartLocation = GetActorLocation() + MovementDirection * 50.0f;
		FVector RotatedVector1 = UKismetMathLibrary::RotateAngleAxis(MovementDirection, 90.0f, GetActorRightVector());
		EndLocation = StartLocation + RotatedVector1 * 100.0f;
		GetWorld()->LineTraceSingleByChannel(OuterHitResult1, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility);
		//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor(255, 0, 0), false, 2.0f, 0,2.0f);
		if(OuterHitResult1.bBlockingHit)
		{
			FVector RotatedVector2 = UKismetMathLibrary::RotateAngleAxis(MovementDirection, 85.0f, GetActorRightVector());
			EndLocation = StartLocation + RotatedVector2 * 200.0f;
			GetWorld()->LineTraceSingleByChannel(OuterHitResult2, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility);
			//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor(255, 0, 0), false, 2.0f, 0,2.0f);
			if(OuterHitResult2.bBlockingHit)
			{
				FVector RotatedVector3 = UKismetMathLibrary::RotateAngleAxis(RotatedVector1, 5.0f, MovementDirection);
				EndLocation = StartLocation + RotatedVector3 * 200.0f;
				GetWorld()->LineTraceSingleByChannel(OuterHitResultR, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility);
				//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor(255, 0, 0), false, 2.0f, 0,2.0f);
				if(OuterHitResultR.bBlockingHit)
				{
					TargetLocation = OuterHitResult1.Location;
					TargetRotation = UKismetMathLibrary::MakeRotationFromAxes(
						UKismetMathLibrary::GetDirectionUnitVector(OuterHitResult1.Location, OuterHitResult2.Location),
						UKismetMathLibrary::GetDirectionUnitVector(OuterHitResult1.Location, OuterHitResultR.Location),
						OuterHitResult1.Normal);
				}
			}
		}
	}
}

void AFESpiderRobotPawn::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if(!bIsTerminated && Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		TraceSurface(MovementVector);
		
		if(UKismetMathLibrary::Vector_Distance(TargetLocation, GetActorLocation()) > 2.0f)
		{
			AddMovementInput(UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), TargetLocation), 1.0f);
			SetActorRotation(UKismetMathLibrary::RLerp(GetActorRotation(), TargetRotation, 0.15f, true));
		}
	}

	/*if(!GetMovementComponent()->IsFalling() && MovementState == EFEMovementState::InAir)
	{
		MovementState = EFEMovementState::Grounded;
	}*/
}

void AFESpiderRobotPawn::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (!bIsTerminated && Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFESpiderRobotPawn::Terminate()
{
	bIsTerminated = true;
	PlayerController->FEHUDWidget->UpdateSpiderRobotRemainingTime(0.0f);
	
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				GetController()->Possess(OriginPlayerCharacter);
				PossessArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				PlayerController->FEHUDWidget->PlayAnimation(PlayerController->FEHUDWidget->UpAnimation);
				GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

			}), 3.0f, false);
}