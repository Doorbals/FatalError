// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/FEAICharacterBase.h"
#include "AI/FEAIController.h"
#include "Animation/AnimInstance.h"
#include "Animation/FEAIAnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Character/FEPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/FEWidgetComponent_AIEye.h"

AFEAICharacterBase::AFEAICharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = AFEAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	CarryBodyArea = CreateDefaultSubobject<UBoxComponent>(FName("CarryBody Area"));
	CarryBodyArea->SetupAttachment(RootComponent);
	
	AssassinRep = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Assassin Rep"));
	AssassinRep->SetupAttachment(RootComponent);

	EyeWidget = CreateDefaultSubobject<UFEWidgetComponent_AIEye>(FName("AI Eye Widget"));
	EyeWidget->SetupAttachment(RootComponent);
	
	CarryBodyYOffset = 75.0f;
	CarryBodyXOffset = -25.0f;
}

bool AFEAICharacterBase::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation,
	int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible,
	int32* UserData)
{
	AFEAIController* AIController = Cast<AFEAIController>(GetController());

	if(AIController->CurrentDetectState != EFEDetectState::Red)
	{
		bIsDetectedInRedState = false;
	}
	
	// 시체 상태이거나, 감지 단계 빨강일 때 다른 AI에게 시각 자극 주기 위한 부분
	if(bIsDead && !bIsDeadBodyDetected)
	{
		if((ObserverLocation - GetActorLocation()).Length() <= 1500.0f)
		{
			static const FName NAME_LineOfSight = FName(TEXT("LineOfSight"));
			FHitResult HitResult;

			// ActorLocation으로 트레이스 발사
			const bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult, ObserverLocation, GetActorLocation()
				, FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic) | ECC_TO_BITFIELD(ECC_Pawn)) 
				, FCollisionQueryParams(NAME_LineOfSight, true, IgnoreActor));

			NumberOfLoSChecksPerformed++;

			if(bHit == true && (IsValid(HitResult.GetActor()) && HitResult.GetActor()->IsOwnedBy(this)))
			{
				OutSeenLocation = GetActorLocation();
				OutSightStrength = 1.0f;
				bIsDeadBodyDetected = true;
				return true;
			}
		}
	}
	else if(AIController->CurrentDetectState == EFEDetectState::Red && !bIsDetectedInRedState)
	{
		if((ObserverLocation - GetActorLocation()).Length() <= 1500.0f)
		{
			static const FName NAME_LineOfSight = FName(TEXT("LineOfSight"));
			FHitResult HitResult;
	 
			// ActorLocation으로 트레이스 발사
			const bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult, ObserverLocation, GetActorLocation()
				, FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic) | ECC_TO_BITFIELD(ECC_Pawn)) 
				, FCollisionQueryParams(NAME_LineOfSight, true, IgnoreActor));

			NumberOfLoSChecksPerformed++;

			if(bHit == true && (IsValid(HitResult.GetActor()) && HitResult.GetActor()->IsOwnedBy(this)))
			{
				OutSeenLocation = GetActorLocation();
				OutSightStrength = 1.0f;
				bIsDetectedInRedState = true;
				return true;
			}
		}
	}
	
	return false;
}

// Called when the game starts or when spawned
void AFEAICharacterBase::BeginPlay()
{
	Super::BeginPlay();

	SkeletalMesh = GetMesh();
	AFEAIController* AIController = Cast<AFEAIController>(UAIBlueprintHelperLibrary::GetAIController(GetOwner()));
	if(AIController != nullptr)
	{
		AIDetectionInfo = Cast<IFEAIDetectionInfoInterface>(AIController);
	}
}

void AFEAICharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if(bIsRagDoll)
	{
		if(bIsRecovering)
		{
			PhysicsAlpha = UKismetMathLibrary::FInterpTo(PhysicsAlpha, 0.0f, DeltaSeconds, 5.0f);
			SkeletalMesh->SetAllBodiesBelowPhysicsBlendWeight(PhysicsBoneName, PhysicsAlpha);
		}
		else
		{
			if(SkeletalMesh->GetPhysicsLinearVelocity(FName(PhysicsBoneName)).Length() > 0.0f)
			{
				GetCapsuleComponent()->SetWorldLocation(SkeletalMesh->GetSocketLocation(PhysicsBoneName) + FVector(0.0f, 0.0f, 75.0f));
			}
			else
			{
				SetActorTickEnabled(false);
			}
		}
	}
}

void AFEAICharacterBase::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = GetMesh()->GetSocketLocation("headSocket");
	OutRotation = GetMesh()->GetSocketRotation("headSocket") + FRotator(-60.0f, 0.0f, 0.0f) ;
}

// 알람 울릴 때 데미지로 자극 받음.
float AFEAICharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	UE_LOG(LogTemp, Log, TEXT("Alarm Damage"));
	AIDetectionInfo->SetIsTargetDetected(true);
	AIDetectionInfo->SetIsTargetOutOfSight(false);
	AIDetectionInfo->SetTargetSightStrength(0.7f);
	AIDetectionInfo->SetPredictedDetectState(EFEDetectState::Orange);
	AIDetectionInfo->SetFinalStimulusLocation(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation());

	AAIController* AIController = Cast<AAIController>(GetController());
	AIController->GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	AIController->GetBlackboardComponent()->SetValueAsBool(FName("IsTargetInSight"), false);
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}


// 암살 (뒤에서 공격) 당할 때 실행되는 함수
void AFEAICharacterBase::Assassinated(FVector& Location, FRotator& Rotation)
{
	// BT 동작 정지
	AFEAIController* AIController = Cast<AFEAIController>(GetController());
	if(AIController != nullptr)
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("IsDead"), true);
		AIController->StopAI();
	}
	bIsDead = true;
	
	// 콜리전 변경 (애니메이션 재생할 때 겹쳐야 하므로)
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	UAnimInstance* AnimInstance = SkeletalMesh->GetAnimInstance();
	AnimInstance->Montage_Play(AssassinatedMontage);
	FOnMontageBlendingOutStarted BlendOutDelegate;
	BlendOutDelegate.BindUObject(this, &ThisClass::ExecuteOnAssassinateMontageEnded);
	AnimInstance->Montage_SetBlendingOutDelegate(BlendOutDelegate, AssassinatedMontage);
		
	Location = AssassinRep->GetComponentLocation();
	Rotation = GetActorRotation();

	EyeWidget->DestroyComponent();
}

// 앞에서 공격 당할 때 실행되는 함수
void AFEAICharacterBase::Attacked(FVector& Location, FRotator& Rotation)
{
	// BT 동작 정지
	AFEAIController* AIController = Cast<AFEAIController>(GetController());
	if(AIController != nullptr)
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("IsDead"), true);
		AIController->StopAI();
	}
	bIsDead = true;
	
	// 콜리전 변경 (애니메이션 재생할 때 겹쳐야 하므로)
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	UAnimInstance* AnimInstance = SkeletalMesh->GetAnimInstance();
	AFEPlayerCharacter* PlayerCharacter = Cast<AFEPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if(PlayerCharacter == nullptr)
	{
		return;
	}
	
	uint32 MontagesOrder = PlayerCharacter->AttackMontagesOrder;
	AnimInstance->Montage_Play(AttackedMontages[MontagesOrder]);
	FOnMontageBlendingOutStarted BlendOutDelegate;
	BlendOutDelegate.BindUObject(this, &ThisClass::ExecuteOnAssassinateMontageEnded);
	AnimInstance->Montage_SetBlendingOutDelegate(BlendOutDelegate, AttackedMontages[MontagesOrder]);

	// Motion Warp 이동 위치 설정을 위해 Offset 조정
	Rotation = FRotator(0.0f, GetActorRotation().Yaw + 180.0f, 0.0f);
	AttackXOffset = 0.0f;
	if(PlayerCharacter->AttackMontagesOrder < 2)
	{
		AttackYOffset = -90.0f;
		Rotation = GetActorRotation();
	}
	else if(PlayerCharacter->AttackMontagesOrder == 2)
	{
		AttackYOffset = 110.0f;
	}
	else if(PlayerCharacter->AttackMontagesOrder == 3)
	{
		AttackYOffset = 110.0f;
	}
	else if(PlayerCharacter->AttackMontagesOrder == 4)
	{
		AttackYOffset = 60.0f;
		AttackXOffset = -15.0f;
	}
	else if(PlayerCharacter->AttackMontagesOrder == 5)
	{
		AttackYOffset = 90.0f;
	}
	else if(PlayerCharacter->AttackMontagesOrder == 6)
	{
		AttackYOffset = 60.0f;
	}
	
	Location = GetActorLocation() + (GetActorForwardVector() * AttackYOffset) + (GetActorRightVector() * AttackXOffset);
	EyeWidget->DestroyComponent();
}

// 공격 당하는 애니메이션 몽타주 블랜드아웃 될 때 실행되는 함수
void AFEAICharacterBase::ExecuteOnAssassinateMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted)
{
	SkeletalMesh->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
 	SkeletalMesh->SetAllBodiesBelowSimulatePhysics(PhysicsBoneName, true);
	SkeletalMesh->SetAllBodiesBelowPhysicsBlendWeight(PhysicsBoneName, 1.0f);
	bIsRagDoll = true;
	SetActorTickEnabled(true);
	PhysicsAlpha = 1.0f;

	// 해당 설정 해주어야 Attach 된 이후 플레이어가 회전하는 방향으로 함께 회전함
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	bUseControllerRotationYaw = true;
}

// 시체 들어 올려질 때 실행되는 함수
void AFEAICharacterBase::CarriedByPlayer(FVector& Location, FRotator& Rotation)
{
	SetActorTickEnabled(true);
	bIsRecovering = true;
	PhysicsAlpha = 1.0f;
	
	FTimerHandle RecoveringTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(RecoveringTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				bIsRecovering = false;
				bIsRagDoll = false;
				SkeletalMesh->SetAllBodiesBelowSimulatePhysics(PhysicsBoneName, false);
				SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				GetWorld()->GetTimerManager().ClearTimer(RecoveringTimerHandle);

			}), 1.0f, false);
	
	SkeletalMesh->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	UAnimInstance* AnimInstance = SkeletalMesh->GetAnimInstance();
	AnimInstance->Montage_Play(CarriedBodyMontage);
	FOnMontageBlendingOutStarted BlendOutDelegate;
	BlendOutDelegate.BindUObject(this, &ThisClass::ExecuteOnCarryBodyMontageEnded);
	AnimInstance->Montage_SetBlendingOutDelegate(BlendOutDelegate, CarriedBodyMontage);
	
	Location = GetActorLocation() - GetActorForwardVector() * CarryBodyYOffset - GetActorRightVector() * CarryBodyXOffset; 
	Rotation = GetActorRotation(); 
}

// 시체 들어 올려지는 애니메이션 몽타주 블랜드아웃 될 때 실행되는 함수
void AFEAICharacterBase::ExecuteOnCarryBodyMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted)
{
	bIsCarried = true;
	Cast<UFEAIAnimInstance>(SkeletalMesh->GetAnimInstance())->AnimState = EFEAIAnimState::Idle; 
	Cast<UFEAIAnimInstance>(SkeletalMesh->GetAnimInstance())->bIsCarried = true; 
}

// 시체 땅에 내려 놓아질 때 실행되는 함수
void AFEAICharacterBase::DroppedByPlayer(FVector& Location, FRotator& Rotation)
{
	UAnimInstance* AnimInstance = SkeletalMesh->GetAnimInstance();
	AnimInstance->Montage_Play(DroppedBodyMontage);
	FOnMontageBlendingOutStarted BlendOutDelegate;
	BlendOutDelegate.BindUObject(this, &ThisClass::ExecuteOnDropBodyMontageEnded);
	AnimInstance->Montage_SetBlendingOutDelegate(BlendOutDelegate, DroppedBodyMontage);
}

// 시체 땅에 내려놓아지는 애니메이션 몽타주 블랜드아웃 될 때 실행되는 함수
void AFEAICharacterBase::ExecuteOnDropBodyMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted)
{
	bIsCarried = false;
	Cast<UFEAIAnimInstance>(SkeletalMesh->GetAnimInstance())->bIsCarried = false;
	SkeletalMesh->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SkeletalMesh->SetAllBodiesBelowSimulatePhysics(PhysicsBoneName, true);
	SkeletalMesh->SetAllBodiesBelowPhysicsBlendWeight(PhysicsBoneName, 1.0f);
	bIsRagDoll = true;
	SetActorTickEnabled(true);
	PhysicsAlpha = 1.0f;
}

