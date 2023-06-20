// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/PlayerAbilities/FEGameplayAbility_Sneak.h"
#include "Character/FEPlayerCharacter.h"

UFEGameplayAbility_Sneak::UFEGameplayAbility_Sneak()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Sneak")));
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Input.Ability.Sneak")));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Jump")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));
}

void UFEGameplayAbility_Sneak::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if(!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	AFEPlayerCharacter* Character = CastChecked<AFEPlayerCharacter>(ActorInfo->AvatarActor.Get());
	Character->WallTrace();
}

bool UFEGameplayAbility_Sneak::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	
	const AFEPlayerCharacter* Character = CastChecked<AFEPlayerCharacter>(ActorInfo->AvatarActor.Get());
	return !Character->bIsSneak && Character->MovementState == EFEMovementState::Grounded;
}

void UFEGameplayAbility_Sneak::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if(ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

void UFEGameplayAbility_Sneak::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	AFEPlayerCharacter* Character = CastChecked<AFEPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if(Character->bIsSneak)
	{
		Character->EndSneak();
	}
}
