// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/PlayerAbilities/FEGameplayAbility_Jump.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/FEPlayerCharacter.h"
#include "Character/Abilities/FEAbilitySystemComponent.h"
#include "Character/Abilities/AbilityTasks/FEAT_WaitInputRelease.h"
#include "FatalError/FatalError.h"

UFEGameplayAbility_Jump::UFEGameplayAbility_Jump()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Jump")));
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Input.Ability.Jump")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Assassinate")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Assassinate")));
}

void UFEGameplayAbility_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if(!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	AFEPlayerCharacter* Character = CastChecked<AFEPlayerCharacter>(    ActorInfo->AvatarActor.Get());
	Character->Jump();
}

bool UFEGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	
	const AFECharacterBase* Character = CastChecked<AFECharacterBase>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	return Character && Character->CanJump();
}

void UFEGameplayAbility_Jump::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if(ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

void UFEGameplayAbility_Jump::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	AFEPlayerCharacter* Character = CastChecked<AFEPlayerCharacter>(ActorInfo->AvatarActor.Get());
	Character->StopJumping();
}

