// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/PlayerAbilities/FEGameplayAbility_Jump.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/FEPlayerCharacter.h"
#include "Character/Abilities/FEAbilitySystemComponent.h"
#include "FatalError/FatalError.h"

UFEGameplayAbility_Jump::UFEGameplayAbility_Jump()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Jump")));
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Input.Ability.Jump")));
	static const FName NAME_Ability(TEXT("Ability"));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(NAME_Ability));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(NAME_Ability));
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
	if(Character == nullptr)
	{
		return;
	}
	
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
	if(Character == nullptr)
	{
		return false;
	}
	
	return Character && Character->CanJump();
}

void UFEGameplayAbility_Jump::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if(ActorInfo && ActorInfo->AvatarActor != nullptr)
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
	if(Character == nullptr)
	{
		return;
	}
	
	Character->StopJumping();
}

