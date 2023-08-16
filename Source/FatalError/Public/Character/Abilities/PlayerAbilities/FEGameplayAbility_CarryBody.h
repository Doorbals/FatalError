// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Abilities/FEGameplayAbility.h"
#include "FEGameplayAbility_CarryBody.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UFEGameplayAbility_CarryBody : public UFEGameplayAbility
{
	GENERATED_BODY()

public:
	UFEGameplayAbility_CarryBody();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	UFUNCTION()
	void EndAbilityForDelegate();

	UFUNCTION()
	void CarryBody();
	
	class AFEAICharacterBase* AICharacter;

	class UAbilityTask_PlayMontageAndWait* MontageTask;

	UAnimMontage* MontageToPlay;
};
