// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FEPlayerController.h"
#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Character/Abilities/FEAbilitySystemComponent.h"
#include "Player/FEPlayerState.h"
#include "UI/FEHUDWidget.h"

AFEPlayerController::AFEPlayerController()
{
	TeamId = 0;

	static ConstructorHelpers::FClassFinder<UFEHUDWidget> FEHUDWidgetRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Characters/Player/UI/WBP_FEHUD.WBP_FEHUD_C'"));
	if(FEHUDWidgetRef.Class)
	{
		FEHUDWidgetClass = FEHUDWidgetRef.Class;
	}
}

FGenericTeamId AFEPlayerController::GetGenericTeamId() const
{
	return TeamId;
}

void AFEPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FEHUDWidget = CreateWidget<UFEHUDWidget>(this, FEHUDWidgetClass);
	if(FEHUDWidget)
	{
		FEHUDWidget->AddToViewport();
	}
}

// 플레이어 컨트롤러가 새로운 Pawn을 소유할 때 호출된다. 일반적으로 플레이어 컨트롤러와 관련된 작업 초기화에 사용된다.
void AFEPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AFEPlayerState* PS = GetPlayerState<AFEPlayerState>();
    if (PS)
    {
    	// 어빌리티 시스템 컴포넌트의 액터를 초기화. (Owner : PlayerState / AvatarActor : Pawn으로 정보 설정)
    	PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
    }
}

// 플레이어의 입력이 받아지고 처리된 후 호출되는 함수. 여기에서 AbilitySystem의 ProcessAbilityInput 함수 실행
void AFEPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if(UFEAbilitySystemComponent* ASC = GetFEAbilitySystemComponent())
	{
		ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
	Super::PostProcessInput(DeltaTime, bGamePaused);
}

UFEAbilitySystemComponent* AFEPlayerController::GetFEAbilitySystemComponent()
{
	AFEPlayerState* PS = GetPlayerState<AFEPlayerState>();
	return Cast<UFEAbilitySystemComponent>(PS->GetAbilitySystemComponent()); 
}
