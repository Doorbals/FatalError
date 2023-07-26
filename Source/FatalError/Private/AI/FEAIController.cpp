// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FEAIController.h"
#include "AI/FEAICharacter_Combat.h"
#include "AI/FEAIPerceptionComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/FEPlayerCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AISenseConfig_Damage.h"

AFEAIController::AFEAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UFEAIPerceptionComponent>("AI Perception Component");

	// Sight 관련 설정
	AISenseConfigSight = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight Sense");
	// 해당 AI에게 인식되는 대상 설정. 해당 대상만 Sight를 트리거 할 수 있음.
	AISenseConfigSight->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfigSight->DetectionByAffiliation.bDetectFriendlies = true;
	AISenseConfigSight->DetectionByAffiliation.bDetectNeutrals = false;
	AISenseConfigSight->SightRadius = 1500.0f;
	AISenseConfigSight->LoseSightRadius = 1700.0f;
	AISenseConfigSight->PeripheralVisionAngleDegrees = 50.0f;
	AISenseConfigSight->PointOfViewBackwardOffset = 100.0f;
	AISenseConfigSight->NearClippingRadius = 100.0f;
	AISenseConfigSight->SetMaxAge(1.0f);

	// Hearing 관련 설정
	AISenseConfigHearing = CreateDefaultSubobject<UAISenseConfig_Hearing>("Hearing Sense");
	// 해당 AI에게 인식되는 대상 설정. 해당 대상만 Hearing을 트리거 할 수 있음.
	AISenseConfigHearing->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfigHearing->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseConfigHearing->DetectionByAffiliation.bDetectNeutrals = true;
	AISenseConfigHearing->HearingRange = 1200.0f;
	AISenseConfigHearing->SetMaxAge(0.5f);

	// Damage 관련 설정
	AISenseConfigDamage = CreateDefaultSubobject<UAISenseConfig_Damage>("Damage Sense");

	// Sight, Hearing을 AIPerceptionComponent가 관리하도록 설정
	AIPerceptionComponent->ConfigureSense(*AISenseConfigSight);
	AIPerceptionComponent->ConfigureSense(*AISenseConfigHearing);
	AIPerceptionComponent->ConfigureSense(*AISenseConfigDamage);
	// Sight를 최우선 감각으로 설정
	AIPerceptionComponent->SetDominantSense(AISenseConfigSight->GetSenseImplementation());

	// AI의 팀 ID를 1로 설정
	AAIController::SetGenericTeamId(FGenericTeamId(1));

	CurrentDetectState = EFEDetectState::Green;
	PredictedDetectState = EFEDetectState::Yellow;
	bIsTargetDetected = false;
	bIsHearingSound = false;
	TargetSightStrength = 0.0f;
	TargetHearingStrength = 0.0f;
	bIsPercentFull = false;
	bIsTargetOutOfSight = true;
}

void AFEAIController::BeginPlay()
{
	Super::BeginPlay();
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::OnTargetPerceptionUpdated_Delegate);
	AIPerceptionComponent->OnStimulusExpired.AddDynamic(this, &ThisClass::OnStimulusExpired_Delegate);
	Cast<AFEPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->TargetOnUpdated.AddDynamic(this, &ThisClass::CalculateSightStrength_Delegate);
	SetCurrentDetectState(EFEDetectState::Green);
}

void AFEAIController::RunAI()
{
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	// 특정 Blackboard 에셋 사용하여 BlackboardComponent 생성
	if(UseBlackboard(BBAsset, BlackboardPtr))
	{
		// 특정 비헤이비어 트리 에셋 사용하여 비헤이비어 트리 실행
		bool RunResult = RunBehaviorTree(BTAsset);
		ensure(RunResult);
	}
}
 
void AFEAIController::StopAI()
{
	// BrainComponent를 비헤이비어트리 컴포넌트로 Cast
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if(BTComponent && BTComponent->IsRunning())
	{
		// 비헤이비어 트리 종료
		BTComponent->StopTree();
	}
}

void AFEAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

// Perception으로 감지된 대상의 팀을 확인해 피아식별
ETeamAttitude::Type AFEAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if(APawn const* OtherPawn = Cast<APawn>(&Other))
	{
		if(auto const TeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))
		{
			if(TeamAgent->GetGenericTeamId() == FGenericTeamId(1))
			{
				return ETeamAttitude::Friendly;
			}
			else if(TeamAgent->GetGenericTeamId() == FGenericTeamId(0))
			{
				return ETeamAttitude::Hostile;
			}
		}
	}
	return ETeamAttitude::Neutral;
}

// 타겟에 대한 정보가 업데이트 될 때 실행되는 델리게이트 함수
void AFEAIController::OnTargetPerceptionUpdated_Delegate(AActor* Actor, FAIStimulus Stimulus)
{
	AFEPlayerCharacter* PlayerCharacter = Cast<AFEPlayerCharacter>(Actor);
	if(PlayerCharacter != nullptr)
	{
		// Blackboard의 키 값인 TargetActor를 플레이어 캐릭터로 설정
		Blackboard->SetValueAsObject(FName("TargetActor"), Actor);
	}
	
	switch (Stimulus.Type)
	{
	case 0:
		// Sight에 대한 동작
		if(Stimulus.WasSuccessfullySensed())
		{
			if(PlayerCharacter != nullptr)
			{
				bIsTargetDetected = true;
				bIsTargetOutOfSight = false;

				if(CurrentDetectState == EFEDetectState::Red)
				{
					TargetSightStrength = 1.0f;
				}
				Blackboard->SetValueAsBool(FName("IsTargetInSight"), true);
			}

			AFEAICharacterBase* AICharacter = Cast<AFEAICharacterBase>(Actor);
			if(AICharacter != nullptr)
			{
				bIsTargetDetected = true;
				bIsTargetOutOfSight = false;
				TargetSightStrength = 0.7f;
				PredictedDetectState = EFEDetectState::Orange;
				FinalStimulusLocation = AICharacter->GetActorLocation();
				AICharacter->bIsDeadBodyDetected = true;
			}
		}
		else
		{
			if(!bIsPercentFull)
			{
				bIsTargetDetected = false;
			}
			
			TargetSightStrength = 0.0f;
			bIsTargetOutOfSight = true;
		}
		break;
	case 1:
		// Hearing에 대한 동작
		if(Stimulus.WasSuccessfullySensed())
		{
			if(PlayerCharacter != nullptr)
			{
				bIsHearingSound = true;
				CalculateHearingStrength(Actor->GetActorLocation());
			}
		}
		else
		{
			if(!bIsPercentFull)
			{
				bIsHearingSound = false;
			}
			TargetHearingStrength = 0.0f;
		}
		break;
	default:
		return;
	}
}

// 자극이 사라질 때 실행되는 델리게이트 함수
void AFEAIController::OnStimulusExpired_Delegate(FAIStimulus& StimulusStore)
{
	switch (StimulusStore.Type)
	{
	case 0:
		TargetSightStrength = 0.0f;
		Blackboard->SetValueAsBool(FName("IsTargetInSight"), false);
		break;
	case 1:
		TargetHearingStrength = 0.0f;
		break;
	default:
		return;
	}
}

// AI의 시각에 의해 탐지될 때, FEPlayerCharacter 클래스의 CanBeSeenFrom 함수에서 실행되는 BroadCast에 의해 실행되는 함수
void AFEAIController::CalculateSightStrength_Delegate(const FVector& TargetLocation)
{
	if(bIsTargetOutOfSight)
	{
		TargetSightStrength = 0.0f;
		return;
	} 
	
	FVector VectorToTarget = TargetLocation - GetPawn()->GetActorLocation();
	float DistanceBetween = VectorToTarget.Length();	// AI와 플레이어 사이 거리
	VectorToTarget.Normalize();
	FVector ForwardVector = GetPawn()->GetActorForwardVector();
	ForwardVector.Normalize();
	float AngleBetween = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(VectorToTarget, ForwardVector)));	// AI의 ForwardVector와 플레이어로 향하는 벡터 사이 각도
	
	// AI와 플레이어 사이 거리와 각도를 통해 시각 감지 강도를 설정. (감지 단계에 따라 Multiplier로 감도 강화)
	float DistanceSightStrength = FMath::Clamp((1 / DistanceBetween) * 1500.0f, 1.0f, 2.0f);
	float AngleSightStrength = FMath::Clamp((1 / (AngleBetween + 0.0001f)) * 20.0f, 1.0f, 1.5f);
	if(DistanceSightStrength >= 2.0f)
	{
		SightStrengthMultiplier = 0.35f;
		AngleSightStrength = 1.5f;
	}
	
	TargetSightStrength = FMath::Clamp((DistanceSightStrength * AngleSightStrength) * SightStrengthMultiplier, 0.0f, 1.0f);

	/*UE_LOG(LogTemp, Warning, TEXT("dist : %f"), DistanceSightStrength);
	UE_LOG(LogTemp, Warning, TEXT("ang : %f"), AngleSightStrength);
	UE_LOG(LogTemp, Warning, TEXT("final : %f"), TargetSightStrength);*/
	
	// 현재 감지 강도에 따라 PredictedDetectState(예상 감지 단계)의 값을 설정
	if(TargetSightStrength >= 0.9f)
	{
		PredictedDetectState = EFEDetectState::Red;
	}
	else if(TargetSightStrength >= 0.7f)
	{
		PredictedDetectState = EFEDetectState::Orange;
	}
	else
	{
		PredictedDetectState = EFEDetectState::Yellow;
	}
	
	FinalStimulusLocation = TargetLocation;
}

// AI의 청각에 의해 탐지될 때 실행되는 함수
void AFEAIController::CalculateHearingStrength(const FVector& TargetLocation)
{
	// AI와 플레이어 사이 거리를 통해 청각 감지 강도를 설정
	FVector VectorToTarget = TargetLocation - GetPawn()->GetActorLocation();
	float DistanceBetween = VectorToTarget.Length();	// AI와 플레이어 사이 거리
	TargetHearingStrength = FMath::Clamp((1/ DistanceBetween) * 700.0f, 0.0f, 1.0f);

	// 시야에 들어오지 않은 상태에서만 PredictedDetectState(예상 감지 단계) 설정 -> 시야 감각을 최우선시
	if(!bIsTargetDetected)
	{
		if(TargetHearingStrength > 0.9f)
		{
			PredictedDetectState = EFEDetectState::Red;
		}
		else if(TargetHearingStrength > 0.7f)
		{
			PredictedDetectState = EFEDetectState::Orange;
		}
		else
		{
			PredictedDetectState = EFEDetectState::Yellow;
		}
	}

	// 마지막 자극 위치를 업데이트할 때, 시야 범위에서 벗어날 때만 청각 자극 위치로 지정 (시각이 최우선)
	if(bIsTargetOutOfSight)
	{
		FinalStimulusLocation = TargetLocation;
	}
}

void AFEAIController::SetCurrentDetectState(EFEDetectState InCurrentDetectState)
{
	if(CurrentDetectState != InCurrentDetectState)
	{
		EFEDetectState PrevDetectState = CurrentDetectState;
		// 변경 이전의 State는 Blackboard의 PrevDetectState에 저장
		Blackboard->SetValueAsEnum(FName("PrevDetectState"), static_cast<uint8>(PrevDetectState));
	
		CurrentDetectState = InCurrentDetectState;
		// CurrentState 변경할 때 Blackboard의 키값도 함께 업데이트
		Blackboard->SetValueAsEnum(FName("CurrentDetectState"), static_cast<uint8>(InCurrentDetectState));

		if(PrevDetectState < CurrentDetectState)
		{
			Blackboard->SetValueAsBool(FName("IsStateStepUp"), true);
		}
		else
		{
			Blackboard->SetValueAsBool(FName("IsStateStepUp"), false);
		}

		Blackboard->SetValueAsBool(FName("LoopingCurState"), false);
	}
}
