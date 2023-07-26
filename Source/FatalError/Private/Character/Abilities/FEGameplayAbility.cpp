// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/FEGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"

UFEGameplayAbility::UFEGameplayAbility()
{
	// GameplayAbility의 인스턴스화 방식을 나타내는 열거형 변수
	// InstancedPerActor : AbilitySystemComponent 인스턴스마다 GameplayAbility가 생성.
	// 다른 인스턴스에 영향 주지 않고 한 번에 여러 개의 AbilitySystemComponent에서 동시 실행 가능.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// ActivationBlockedTags : 게임 플레이 어빌리티가 활성화되는 것을 방지하는 태그 집합
	// 어빌리티가 활성화되기 전에 이 집합에 있는 태그가 액터에 적용되어 있다면, 이 어빌리티는 활성화 X
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
}

// 어빌리티가 캐릭터에게 할당될 때 호출
void UFEGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	//  ActivateAbilityOnGranted이 true일 경우 즉시 해당 어빌리티를 활성화.
	if (ActivateAbilityOnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}
