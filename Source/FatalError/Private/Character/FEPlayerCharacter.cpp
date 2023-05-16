// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FEPlayerCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "FEGameplayTags.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "Character/Abilities/AttributeSet/FEAttributeSetBase.h"
#include "Character/Abilities/FEAbilitySystemComponent.h"
#include "Character/Abilities/FEGameplayAbility.h"
#include "Components/CapsuleComponent.h"
#include "FatalError/FatalError.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/FEEnhancedInputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/FEPlayerController.h"
#include "Player/FEPlayerState.h"
#include "UObject/ConstructorHelpers.h"

class AFEPlayerController;

AFEPlayerCharacter::AFEPlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(FName("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	// 뷰포트에 보이지 않을 때도 애니메이션 틱이 발생하여 애니메이션 항상 정확하게 표시.
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC(TEXT("/Game/Characters/Input/IMC_Default.IMC_Default"));
	if (IMC.Succeeded())
	{
		InputMapping = IMC.Object;
	}

	static ConstructorHelpers::FObjectFinder<UFEInputConfig> INPUT_DATA(TEXT("/Game/Characters/Input/DA_InputConfig.DA_InputConfig"));
	if (INPUT_DATA.Succeeded())
	{
		InputConfig = INPUT_DATA.Object;
	}
}

void AFEPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UFEEnhancedInputComponent* EnhancedInputComponent = Cast<UFEEnhancedInputComponent>(PlayerInputComponent);
	check(EnhancedInputComponent);
	
	const FFEGameplayTags& GameplayTags = FFEGameplayTags::Get();

	TArray<uint32> BindHandles;
	EnhancedInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, BindHandles);
	EnhancedInputComponent->BindNativeActions(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &AFEPlayerCharacter::Move);
	EnhancedInputComponent->BindNativeActions(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::None, this, &AFEPlayerCharacter::StopMove);
	EnhancedInputComponent->BindNativeActions(InputConfig, GameplayTags.InputTag_Look, ETriggerEvent::Triggered, this, &AFEPlayerCharacter::Look);
	EnhancedInputComponent->BindNativeActions(InputConfig, GameplayTags.InputTag_Crouch, ETriggerEvent::Started, this, &AFEPlayerCharacter::ToggleCrouch);
}

// 플레이어 캐릭터가 컨트롤러에 의해 조종될 때 호출
void AFEPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	AFEPlayerState* PS = GetPlayerState<AFEPlayerState>();
	if (PS)
	{
		// PlayerState를 가져와 ASC 설정 (서버에서 실행)
		AbilitySystemComponent = Cast<UFEAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// AI 캐릭터와 같이 PlayerController를 가지고 있지 않은 캐릭터에 대해 ASC 초기화.
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		// 어트리뷰트 세트 가져와 AttributeSetBase 설정
		AttributeSetBase = PS->GetAttributeSetBase();

		// 어트리뷰트를 초기화
		InitializeAttributes();

		// DeadTag 카운트를 0으로 설정
		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		// 체력과 에너지를 Max 값으로 설정
		SetHealth(GetMaxHealth());
		SetEnergy(GetMaxEnergy());

		// 리스폰 종료
		AddStartupEffects();
		AddCharacterAbilities();
	}
}

USpringArmComponent* AFEPlayerCharacter::GetCameraBoom()
{
	return CameraBoom;
}

UCameraComponent* AFEPlayerCharacter::GetFollowCamera()
{
	return FollowCamera;
}

float AFEPlayerCharacter::GetStartingCameraBoomArmLength()
{
	return StartingCameraBoomArmLength;
}

FVector AFEPlayerCharacter::GetStartingCameraBoomLocation()
{
	return StartingCameraBoomLocation;
}

void AFEPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitControlMode();
	
	StartingCameraBoomArmLength = CameraBoom->TargetArmLength;
	StartingCameraBoomLocation = CameraBoom->GetRelativeLocation();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	Subsystem->AddMappingContext(InputMapping, 0);
}

void AFEPlayerCharacter::InitControlMode()
{
		// 캐릭터가 플레이어 컨트롤러 회전에 의해 회전되지 않도록 한다.
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;

		// 스프링암이 폰 컨트롤 로테이션을 사용하도록 한다.
		CameraBoom->SetRelativeRotation(FRotator::ZeroRotator);
		CameraBoom->SetRelativeLocation(FVector(0, 0, 68.492264));
		CameraBoom->bUsePawnControlRotation = true;
		CameraBoom->bInheritPitch = true;
		CameraBoom->bInheritYaw = true;
		CameraBoom->bInheritRoll = true;
		CameraBoom->bDoCollisionTest = true;
		CameraBoom->bEnableCameraLag = true;
		CameraBoom->bEnableCameraRotationLag = true;
		CameraBoom->CameraLagSpeed = 5.0f;
		CameraBoom->CameraRotationLagSpeed = 50.0f;
		CameraBoom->CameraLagMaxDistance = 300.0f;
		CameraBoom->TargetArmLength = 350.0f;
		FollowCamera->FieldOfView = 80.0f;
	
		GetCapsuleComponent()->SetHiddenInGame(false);
		GetCapsuleComponent()->SetCapsuleRadius(30.0f);
		GetCapsuleComponent()->SetGenerateOverlapEvents(true);
		
		GetCharacterMovement()->bOrientRotationToMovement = true;		// 캐릭터의 이동방향으로 회전
		GetCharacterMovement()->bUseControllerDesiredRotation = true;	// 마우스 방향으로 회전
		GetCharacterMovement()->GravityScale = 1.75f;
		GetCharacterMovement()->MaxAcceleration = 800.0f;
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		
		// bUseSeparateBrakingFriction을 활성화 해주어야 BrakingFriction이 작동한다.
		// 가속이 0인 경우를 제동이라고 하는데, 제동 시에는 Braking Friction 값 * Braking Friction Factor 값으로 마찰계수가 정해진다.
		GetCharacterMovement()->bUseSeparateBrakingFriction = true;
		GetCharacterMovement()->BrakingFriction = 6.0f;
		GetCharacterMovement()->BrakingFrictionFactor = 0.9f;
		GetCharacterMovement()->BrakingDecelerationWalking = 1.0f;
		GetCharacterMovement()->PerchRadiusThreshold = 12.0f;
		GetCharacterMovement()->PerchAdditionalHeight = 5.0f;
		GetCharacterMovement()->GroundFriction = 20.0f;
		
		GetCharacterMovement()->bApplyGravityWhileJumping;
		GetCharacterMovement()->JumpZVelocity = 800.0f;
		GetCharacterMovement()->AirControl = 1.0f;
		
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 550.0f, 0.0f);	// 캐릭터 회전 속도
		GetCharacterMovement()->NavAgentProps.bCanCrouch = true;		// Crouch() 사용하기 위해 true로 설정해아 함.
		GetCharacterMovement()->bCanWalkOffLedges = true;
		GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;	// 난간에서도 크라우칭 상태로 걸을 수 있게 설정
		GetCharacterMovement()->SetCrouchedHalfHeight(GetCapsuleComponent()->GetScaledCapsuleHalfHeight() / 2);		// 크라우칭 상태에서 캡슐 높이를 기존 캡슐 높이의 절반으로 설정
		MovementState = EFEMovementState::Grounded;
}

void AFEPlayerCharacter::SetControlMode(EFEMovementState InMovementState)
{
	switch (InMovementState)
	{
		case EFEMovementState::Grounded:
		{
			GetCharacterMovement()->bUseControllerDesiredRotation = true;
			GetCharacterMovement()->bOrientRotationToMovement = true;
			GetCharacterMovement()->MaxAcceleration = 800.0f;
			GetCharacterMovement()->MaxWalkSpeed = 600.0f;
			GetCharacterMovement()->BrakingFriction = 6.0f;
		}
		break;
		case EFEMovementState::Sneak:
		{
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			GetCharacterMovement()->bOrientRotationToMovement = false;
			GetCharacterMovement()->MaxAcceleration = 100.0f;
			GetCharacterMovement()->MaxWalkSpeed = 100.0f;
			GetCharacterMovement()->BrakingFriction = 10.0f;
		}
		break;
		case EFEMovementState::Climb:
		{
			GetCharacterMovement()->bUseControllerDesiredRotation = false;
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}
		break;
	}
}

void AFEPlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if(IsAlive() && Controller != nullptr)
	{
		switch (MovementState)
		{
			case EFEMovementState::Grounded:
			{
				const FRotator Rotation = Controller->GetControlRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);

				const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
				const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			
				AddMovementInput(ForwardDirection, MovementVector.Y);
				AddMovementInput(RightDirection, MovementVector.X);
			}
			break;
			case EFEMovementState::Sneak:
			{
				SneakTrace(MovementVector.X, MovementVector.Y);
				if(MovementVector.X == 0.0f)
				{
					AddMovementInput(GetCharacterMovement()->GetPlaneConstraintNormal() * -1.0f);
				}
			}
			break;	
			case EFEMovementState::Climb:
			{
				MovingLeftRight = MovementVector.X * 0.2f;
				MovingUpDown = MovementVector.Y;
				if(bCanMoveRight && MovingLeftRight > 0.0f)
				{
					AddMovementInput(WorldDirection, MovingLeftRight);
				}
				if(bCanMoveLeft && MovingLeftRight < 0.0f)
				{
					AddMovementInput(WorldDirection, MovingLeftRight);
				}
			}
			break;
			case EFEMovementState::InAir:
			{
				if(!GetMovementComponent()->IsFalling())
				{
					MovementState = EFEMovementState::Grounded;
				}
			}
		}
	}
}

void AFEPlayerCharacter::StopMove(const FInputActionValue& Value)
{
	MovingLeftRight = 0;
}

void AFEPlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (IsAlive() && Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFEPlayerCharacter::Jump()
{
	if(IsAlive())
	{
		Super::Jump();
		MovementState = EFEMovementState::InAir;
	}
}

void AFEPlayerCharacter::StopJumping()
{
	if(IsAlive())
	{
		Super::StopJumping();
	}
}

void AFEPlayerCharacter::ToggleCrouch()
{
	if(!bIsSneak)
	{
		if (bIsCrouched || GetCharacterMovement()->bWantsToCrouch)
		{
			UnCrouch();
		}
		else if (GetMovementComponent()->IsMovingOnGround())
		{
			Crouch();
		}
	}
}

void AFEPlayerCharacter::WallTrace()
{
	FHitResult HitResult;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + GetActorForwardVector() * 50.0f;

	FCollisionQueryParams CollisionParams;
	CollisionParams.bTraceComplex = true;
	CollisionParams.AddIgnoredActor(this);

	// 라인 트레이스로 캐릭터의 위치에서 전방으로 충돌하는 오브젝트 탐색 (ECC_GameTraceChannel1 : 프로젝트 세팅에서 설정해 놓은 TraceChannel)
	if(GetWorld()->LineTraceSingleByChannel(HitResult,StartLocation, EndLocation, ECC_GameTraceChannel1, CollisionParams))
	{
		StartSneak(HitResult.Normal);
	}
	else
	{
		EndSneak();
	}
}

void AFEPlayerCharacter::StartSneak(FVector PlaneNormal)
{
	// 아래의 방향 제한을 위해 먼저 이를 활성화 해야한다.
	GetCharacterMovement()->SetPlaneConstraintEnabled(true);
	// 캐릭터의 이동 방향을 PlaneNoraml의 평면으로 제한한다. (PlaneNormal을 Normal로 가지는 평면에서만 움직일 수 있음.)
	GetCharacterMovement()->SetPlaneConstraintNormal(PlaneNormal);
	bIsSneak = true;
	MovementState = EFEMovementState::Sneak;
	SetControlMode(MovementState);
	SetActorRotation((PlaneNormal * -1.0f).Rotation());
	Crouch();
}

void AFEPlayerCharacter::EndSneak()
{
	GetCharacterMovement()->SetPlaneConstraintEnabled(false);
	bIsSneak = false;
	MovementState = EFEMovementState::Grounded;
	SetControlMode(MovementState);
	UnCrouch();
} 

void AFEPlayerCharacter::SneakTrace(float InMoveRight, float InMoveForward)
{
	FVector PlaneConstraintNormal = GetCharacterMovement()->GetPlaneConstraintNormal();
	FCollisionQueryParams CollisionParams;
	CollisionParams.bTraceComplex = true;
	CollisionParams.AddIgnoredActor(this);
	
	FHitResult HitResult_R;
	FVector StartLocation_R = GetActorLocation() + GetActorRightVector() * -1.0f * 70.0f; //UKismetMathLibrary::GetRightVector(UKismetMathLibrary::MakeRotFromX(PlaneConstraintNormal * -1.0f)) * 70.0f; 
	FVector EndLocation_R = StartLocation_R + PlaneConstraintNormal * -200.0f;
	bool RightHit = GetWorld()->LineTraceSingleByChannel(HitResult_R,StartLocation_R, EndLocation_R, ECC_GameTraceChannel1, CollisionParams);

	FHitResult HitResult_L;
	FVector StartLocation_L = GetActorLocation() + GetActorRightVector() * 70.0f; //UKismetMathLibrary::GetRightVector(UKismetMathLibrary::MakeRotFromX(PlaneConstraintNormal)) * 70.0f; 
	FVector EndLocation_L = StartLocation_L + PlaneConstraintNormal * -200.0f;
	bool LeftHit = GetWorld()->LineTraceSingleByChannel(HitResult_L, StartLocation_L, EndLocation_L, ECC_GameTraceChannel1, CollisionParams);

	const FVector MoveDirection = GetActorRightVector();
	if(MoveDirection.Y > 0.0f)
	{
		bSneakRight = true;
	}
	else
	{
		bSneakRight = false;
	}
	
	FVector CharacterToCamera = FollowCamera->GetComponentLocation() - GetActorLocation();
	CharacterToCamera.Normalize();
	FVector Cross = UKismetMathLibrary::Cross_VectorVector(GetActorForwardVector(), CharacterToCamera);
	float DotLeftRight = UKismetMathLibrary::Dot_VectorVector(GetActorUpVector(), Cross);
	float DotFrontBack = UKismetMathLibrary::Dot_VectorVector(GetActorForwardVector(), CharacterToCamera);
	InMoveForward *= 10.0f;

	// 벽 안쪽에 있을 때
	if(RightHit && LeftHit)
	{
		if(InMoveRight != 0.0f || InMoveForward != 0.0f)
		{
			FHitResult HitResult;
			FVector StartLocation = GetActorLocation();
			FVector EndLocation = StartLocation + (GetCharacterMovement()->GetPlaneConstraintNormal()) * -200.0f;
			if(GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_GameTraceChannel1, CollisionParams))
			{
				GetCharacterMovement()->SetPlaneConstraintNormal(HitResult.Normal);

				if(DotFrontBack > 0.1f) // 카메라가 캐릭터의 앞에 있을 때
				{
					if(DotLeftRight > 0.2f) // 카메라가 캐릭터의 오른쪽에 있을 때
					{
						AddMovementInput(MoveDirection, -InMoveForward);
						AddMovementInput(MoveDirection, -InMoveRight);
					}
					else if(0.2f >= DotLeftRight && DotLeftRight >= -0.2f) // 카메라가 캐릭터와 마주볼 때
					{
						AddMovementInput(MoveDirection, -InMoveRight);
					}
					else // 카메라가 캐릭터의 왼쪽에 있을 때
					{
						AddMovementInput(MoveDirection, InMoveForward);
						AddMovementInput(MoveDirection, -InMoveRight);
					}
				}
				else if(0.1f >= DotFrontBack && DotFrontBack >= -0.1f) // 카메라가 캐릭터의 옆에 있을 때
				{
					if(DotLeftRight > 0.0f)
					{
						AddMovementInput(MoveDirection, -InMoveForward);
					}
					else
					{
						AddMovementInput(MoveDirection, InMoveForward);
					}
				}
				else // 카메라가 캐릭터의 뒤에 있을 때
				{
					if(DotLeftRight > 0.2f)
					{
						AddMovementInput(MoveDirection, -InMoveForward);
						AddMovementInput(MoveDirection, InMoveRight);
					}
					else if(0.2f >= DotLeftRight && DotLeftRight >= -0.2f)
					{
						AddMovementInput(MoveDirection, InMoveRight);
					}
					else
					{
						AddMovementInput(MoveDirection, InMoveForward);
						AddMovementInput(MoveDirection, InMoveRight);
					}
				}
			}
		}
	}
	else	// 벽 가장자리에 도달했을 때
	{
		float SignMoveRight = UKismetMathLibrary::SignOfFloat(InMoveRight);
		float SignMoveForward = UKismetMathLibrary::SignOfFloat(InMoveForward);
		bool bHitDirectionRight, bHitDirectionForward;
		float RightValue, ForwardValue;
		
		if(DotFrontBack > 0.1f) // 카메라가 캐릭터의 앞에 있을 때
		{
			bHitDirectionRight = (SignMoveRight == 1.0f) ? RightHit : LeftHit;
			RightValue = (SignMoveRight != 0 && bHitDirectionRight) ? InMoveRight : 0.0f;
			if(DotLeftRight > 0.2f) // 카메라가 캐릭터의 오른쪽에 있을 때
			{
				bHitDirectionForward = (SignMoveForward == 1.0f) ? RightHit : LeftHit;
				ForwardValue = (SignMoveForward != 0 && bHitDirectionForward) ? InMoveForward : 0.0f;
				AddMovementInput(MoveDirection, -ForwardValue);
				AddMovementInput(MoveDirection, -RightValue);
			}
			else if(0.2f >= DotLeftRight && DotLeftRight >= -0.2f) // 카메라가 캐릭터와 마주볼 때
			{
				AddMovementInput(MoveDirection, -RightValue);
			}
			else // 카메라가 캐릭터의 왼쪽에 있을 때
			{
				bHitDirectionForward = (SignMoveForward == 1.0f) ? LeftHit : RightHit;
				ForwardValue = (SignMoveForward != 0 && bHitDirectionForward) ? InMoveForward : 0.0f;
				AddMovementInput(MoveDirection, ForwardValue);
				AddMovementInput(MoveDirection, -RightValue);
			}
		}
		else if(0.1f >= DotFrontBack && DotFrontBack >= -0.1f ) // 카메라가 캐릭터의 옆에 있을 때
		{
			bHitDirectionRight = (SignMoveRight == 1.0f) ? RightHit : LeftHit;
			RightValue = (SignMoveRight != 0 && bHitDirectionRight) ? InMoveRight : 0.0f;
			if(DotLeftRight > 0.2f)
			{
				bHitDirectionForward = (SignMoveForward == 1.0f) ? RightHit : LeftHit;
				ForwardValue = (SignMoveForward != 0 && bHitDirectionForward) ? InMoveForward : 0.0f;
				AddMovementInput(MoveDirection, -ForwardValue);
			}
			else
			{
				bHitDirectionForward = (SignMoveForward == 1.0f) ? LeftHit : RightHit;
				ForwardValue = (SignMoveForward != 0 && bHitDirectionForward) ? InMoveForward : 0.0f;
				AddMovementInput(MoveDirection, ForwardValue);
			}
		}
		else // 카메라가 캐릭터의 뒤에 있을 때
		{
			bHitDirectionRight = (SignMoveRight == 1.0f) ? LeftHit : RightHit;
			RightValue = (SignMoveRight != 0 && bHitDirectionRight) ? InMoveRight : 0.0f;
			if(DotLeftRight > 0.2f)
			{
				bHitDirectionForward = (SignMoveForward == 1.0f) ? RightHit : LeftHit;
				ForwardValue = (SignMoveForward != 0 && bHitDirectionForward) ? InMoveForward : 0.0f;
				AddMovementInput(MoveDirection, -ForwardValue);
				AddMovementInput(MoveDirection, RightValue);
			}
			else if(0.2f >= DotLeftRight && DotLeftRight >= -0.2f)
			{
				AddMovementInput(MoveDirection, RightValue);
			}
			else
			{
				bHitDirectionForward = (SignMoveForward == 1.0f) ? LeftHit : RightHit;
				ForwardValue = (SignMoveForward != 0 && bHitDirectionForward) ? InMoveForward : 0.0f;
				AddMovementInput(MoveDirection, ForwardValue);
				AddMovementInput(MoveDirection, RightValue);
			}
		}
	}
}

void AFEPlayerCharacter::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	UFEAbilitySystemComponent* ASC = Cast<UFEAbilitySystemComponent>(GetAbilitySystemComponent());
	if (!ASC)
	{
		return;
	}
	ASC->AbilityInputTagPressed(InputTag);
}

void AFEPlayerCharacter::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	UFEAbilitySystemComponent* ASC = Cast<UFEAbilitySystemComponent>(GetAbilitySystemComponent());
	if (!ASC)
	{
		return;
	}
	ASC->AbilityInputTagReleased(InputTag);
}

// 클라이언트에게서 PlayerState가 리플리케이트 되어 도착할 때마다 실행
void AFEPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AFEPlayerState* PS = GetPlayerState<AFEPlayerState>();
	if (PS)
	{
		// 클라이언트용 ASC 설정
		AbilitySystemComponent = Cast<UFEAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// 클라이언트용 ASC에서 액터 정보를 초기화
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);

		// 어트리뷰트 세트 가져와 AttributeSetBase 설정
		AttributeSetBase = PS->GetAttributeSetBase();
		
		// DeadTag 카운트를 0으로 설정
		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);
		
		// 어트리뷰트를 초기화
		InitializeAttributes();

		// 체력과 에너지를 Max 값으로 설정
		SetHealth(GetMaxHealth());
		SetEnergy(GetMaxEnergy());
	}
}
