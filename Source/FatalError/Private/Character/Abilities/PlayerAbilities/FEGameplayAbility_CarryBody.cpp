// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/PlayerAbilities/FEGameplayAbility_CarryBody.h"
#include "MotionWarpingComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AI/FEAICharacterBase.h"
#include "Animation/FEAIAnimInstance.h"
#include "Character/FEPlayerCharacter.h"
#include "Character/Abilities/AbilityInterface/FEI_CarryBody.h"
#include "GameFramework/CharacterMovementComponent.h"

UFEGameplayAbility_CarryBody::UFEGameplayAbility_CarryBody()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.CarryBody")));
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Input.Ability.CarryBody")));
	static const FName NAME_Ability(TEXT("Ability"));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(NAME_Ability));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(NAME_Ability));
}

void UFEGameplayAbility_CarryBody::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
	
	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ImplementedActors;
	Character->GetOverlappingActors(OverlappingActors);
	for (AActor* OverlappingActor : OverlappingActors)
	{
		IFEI_CarryBody* CarryBodyInterface = Cast<IFEI_CarryBody>(OverlappingActor);
		if(CarryBodyInterface != nullptr)
		{
			ImplementedActors.Add(OverlappingActor);
		}
	}
	
	if(ImplementedActors.Num() > 0)
	{
		AICharacter = Cast<AFEAICharacterBase>(ImplementedActors.Top());
		
		if(AICharacter != nullptr)
		{
			if(!AICharacter->bIsDead)
			{
				CancelAbility(Handle, ActorInfo, ActivationInfo, false);
				return;
			}
			
			FVector Location;
			FRotator Rotation;
			UAnimMontage* MontageToPlay;

			Character->GetCharacterMovement()->MaxWalkSpeedCrouched = 0.0f;
			Character->GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f);
			if(!AICharacter->bIsCarried)
			{
				AICharacter->CarriedByPlayer(Location, Rotation);
				Character->MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("Body"), Location, Rotation);
				MontageToPlay = Character->CarryBodyMontage;
				Character->bIsCarryingBody = true;
				Character->Crouch();
			}
			else
			{
				AICharacter->DroppedByPlayer(Location, Rotation);
				MontageToPlay = Character->DropBodyMontage;  
				Character->bIsCarryingBody = false;
				Character->GetCharacterMovement()->StopMovementImmediately();
			}
			
			if(MontageToPlay != nullptr)
			{
				UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, MontageToPlay, 1.0f, NAME_None, true, 1.0f, 0.0f);
				MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::EndAbilityForDelegate);
				MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::EndAbilityForDelegate);
				MontageTask->OnCancelled.AddDynamic(this, &ThisClass::EndAbilityForDelegate);
				MontageTask->OnCompleted.AddDynamic(this, &ThisClass::EndAbilityForDelegate);
				MontageTask->ReadyForActivation();
			}
			
			ImplementedActors.Empty();
		}
	}
	else
	{
		ImplementedActors.Empty();
		CancelAbility(Handle, ActorInfo, ActivationInfo, false);
	}
}

bool UFEGameplayAbility_CarryBody::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UFEGameplayAbility_CarryBody::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
}

void UFEGameplayAbility_CarryBody::EndAbilityForDelegate()
{
	AFEPlayerCharacter* Character = CastChecked<AFEPlayerCharacter>(    CurrentActorInfo->AvatarActor.Get());
	if(Character == nullptr)
	{
		return;
	}
	
	if(Character->bIsCarryingBody)
	{
		FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, true);
		AICharacter->AttachToComponent(Character->GetMesh(), AttachmentTransformRules);
	}
	else
	{
		FDetachmentTransformRules DetachmentTransformRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		AICharacter->DetachFromActor(DetachmentTransformRules);
	}

	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = 300.0f;
	Character->GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f);
	Character->MotionWarpingComponent->RemoveWarpTarget(TEXT("Body"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
