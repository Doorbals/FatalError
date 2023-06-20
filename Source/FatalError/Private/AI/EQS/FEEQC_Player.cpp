// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EQS/FEEQC_Player.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UFEEQC_Player::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	AActor* PlayerCharacter = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)); 
	if(PlayerCharacter != nullptr)
	{
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, PlayerCharacter);
	}
}
