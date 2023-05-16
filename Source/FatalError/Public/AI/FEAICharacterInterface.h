// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FEAICharacterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFEAICharacterInterface : public UInterface
{
	GENERATED_BODY()
};

// 공격이 끝나는 타이밍을 알기 위한 델리게이트
DECLARE_DELEGATE(FAICharacterAttackFinished);

/**
 * 
 */
class FATALERROR_API IFEAICharacterInterface
{
	GENERATED_BODY()

public:
	virtual float GetAIPatrolRadius() = 0;
	virtual float GetAIDecentRange() = 0;
	virtual float GetAIAttackRange() = 0;
	virtual float GetAITurnSpeed() = 0;

	// 공격이 끝나는 타이밍을 알기 위한 델리게이트
	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished) = 0;
	virtual void AttackByAI() = 0;
};
