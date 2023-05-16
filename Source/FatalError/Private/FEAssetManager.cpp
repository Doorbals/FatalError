// Fill out your copyright notice in the Description page of Project Settings.


#include "FEAssetManager.h"
#include "AbilitySystemGlobals.h"
#include "FEGameplayTags.h"

UFEAssetManager::UFEAssetManager()
{
}

UFEAssetManager& UFEAssetManager::Get()
{
	check(GEngine);
	UFEAssetManager* FEAssetManager = Cast<UFEAssetManager>(GEngine->AssetManager);
	return *FEAssetManager;
}

void UFEAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	UAbilitySystemGlobals::Get().InitGlobalData();

	// GameplayTags 싱글턴에서 Native Tag 초기화
	FFEGameplayTags::InitializeNativeTags();
}
