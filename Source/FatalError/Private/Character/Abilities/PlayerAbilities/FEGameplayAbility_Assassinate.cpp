// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Abilities/PlayerAbilities/FEGameplayAbility_Assassinate.h"
#include "MotionWarpingComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AI/FEAICharacterBase.h"
#include "Character/FEPlayerCharacter.h"
#include "Animation/AnimMontage.h"
#include "Character/Abilities/AbilityInterface/FEI_Assassination.h"
#include "Kismet/KismetMathLibrary.h"

UFEGameplayAbility_Assassinate::UFEGameplayAbility_Assassinate()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Assassinate")));
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Input.Ability.Assassinate")));
	static const FName NAME_Ability(TEXT("Ability"));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(NAME_Ability));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(NAME_Ability));
}

void UFEGameplayAbility_Assassinate::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
		IFEI_Assassination* AssassinationInterface = Cast<IFEI_Assassination>(OverlappingActor);
		if(AssassinationInterface != nullptr)
		{
			ImplementedActors.Add(OverlappingActor);
		}
	}
	
	if(ImplementedActors.Num() > 0)
	{
		AFEAICharacterBase* AICharacter = Cast<AFEAICharacterBase>(ImplementedActors.Top());
		
		if(AICharacter != nullptr)
		{
			if(AICharacter->bIsDead)
			{
				CancelAbility(Handle, ActorInfo, ActivationInfo, false);
				return;
			}
		
			FVector Location;
			FRotator Rotation;
			
			FVector AIToPlayer = Character->GetActorLocation() - AICharacter->GetActorLocation();
			AIToPlayer.Normalize();
			UAnimMontage* MontageToPlay; 
			if(UKismetMathLibrary::Dot_VectorVector(AICharacter->GetActorForwardVector(), AIToPlayer) > 0)
			{
				MontageToPlay = Character->AttackMontages[Character->AttackMontagesOrder];
				AICharacter->Attacked(Location, Rotation);
				Character->AttackMontagesOrder = (Character->AttackMontagesOrder + 1) % (Character->AttackMontages).Num();
			}
			else
			{
				MontageToPlay = Character->AssassinateMontage;
				AICharacter->Assassinated(Location, Rotation);
			}
			
			Character->MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("Victim"), Location, Rotation);
			
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

bool UFEGameplayAbility_Assassinate::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	
	return true;
}

void UFEGameplayAbility_Assassinate::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UFEGameplayAbility_Assassinate::EndAbilityForDelegate()
{
	AFEPlayerCharacter* Character = CastChecked<AFEPlayerCharacter>(    CurrentActorInfo->AvatarActor.Get());
	if(Character == nullptr)
	{
		return;
	}
	
	Character->MotionWarpingComponent->RemoveWarpTarget(TEXT("Victim"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
