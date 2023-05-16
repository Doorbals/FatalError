// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FatalError/FatalError.h"
#include "FEGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API UFEGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UFEGameplayAbility();

	// 해당 세트가 존재하는 어빌리티는 input이 눌릴 때 자동으로 활성화 된다.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EFEAbilityInputID AbilityInputID = EFEAbilityInputID::None;

	// 자동으로 활성화된 입력에 연결되지 않고, 슬롯에 기능을 연결하는 값
	// 패시브 어빌리티는 입력에 연결되지 않으므로 일반적으로 어빌리티를 슬롯과 연결하는 방법이 필요하다.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EFEAbilityInputID AbilityID = EFEAbilityInputID::None;

	// 해당 어빌리티가 권한이 부여되면 즉시 활성화 될 수 있는지 알려준다. 패시브 어빌리티와 다른 액터에게 적용하는 어빌리티에 사용된다.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool ActivateAbilityOnGranted = false;

	// OnAvaterSet()은 'ActivateAbilityOnGranted'가 true인 어빌리티를 즉시 실행
	// 패시브나 BeginPlay 타입의 로직을 여기서 수행할 수 있다.
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
};
