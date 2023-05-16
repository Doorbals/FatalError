// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "FEAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UFEAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	UFEAssetManager();

	// 싱글턴 오브젝트를 반환
	static UFEAssetManager& Get();

protected:
	virtual void StartInitialLoading() override;
};
