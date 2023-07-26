// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/PlayerAbilities/FEGameplayAbility_Sneak.h"
#include "Character/FEPlayerCharacter.h"

UFEGameplayAbility_Sneak::UFEGameplayAbility_Sneak()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Sneak")));
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Input.Ability.Sneak")));
	static const FName NAME_Ability(TEXT("Ability"));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(NAME_Ability));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(NAME_Ability));
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
	if(Character == nullptr)
	{
		return;
	}
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
	if(!Character)
	{
		return false;
	}
	
	return !Character->bIsSneak && Character->MovementState == EFEMovementState::Grounded && !Character->bIsCarryingBody;
}

void UFEGameplayAbility_Sneak::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if(ActorInfo && ActorInfo->AvatarActor != nullptr)
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
	if(Character == nullptr)
	{
		return;
	}
	
	if(Character->bIsSneak)
	{
		Character->EndSneak();
	}
}
