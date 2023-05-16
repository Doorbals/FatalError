// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "FECharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, AFECharacterBase*, Character);

UCLASS()
class FATALERROR_API AFECharacterBase : public ACharacter, public IAbilitySystemInterface	// IAbilitySystemInterface 상속 받기 
{
	GENERATED_BODY()

public:
	// 생성자 함수
	AFECharacterBase(const class FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, Category = "FECharacter")
	FCharacterDiedDelegate OnCharacterDied;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "FECharacter")
	virtual bool IsAlive() const;

	// AbilityID를 통해 각각의 어빌리티 레벨을 반환하기 위한 함수로, 모든 어빌리티에 대해 1로 하드코딩 되어있다.
	UFUNCTION(BlueprintCallable, Category = "FECharacter")
	virtual int32 GetAbilityLevel(EFEAbilityInputID AbilityID) const;

	// 캐릭터의 모든 어빌리티들을 지운다. 서버에 의해서만 호출될 수 있으며, 서버에서 삭제되면 클라이언트에서도 삭제된다.
	virtual void RemoveCharacterAbilities();

	/**
	* AttributeSetBase의 어트리뷰트들에 대한 Getter
	**/
	
	UFUNCTION(BlueprintCallable, Category = "FECharacter|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "FECharacter|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "FECharacter|Attributes")
	float GetEnergy() const;

	UFUNCTION(BlueprintCallable, Category = "FECharacter|Attributes")
	float GetMaxEnergy() const;
	
	// Gets the Current value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "FECharacter|Attributes")
	float GetMoveSpeed() const;

	// Gets the Base value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "FECharacter|Attributes")
	float GetMoveSpeedBaseValue() const;
	
	virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "FECharacter")
	virtual void FinishDying();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TWeakObjectPtr<class UFEAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<class UFEAttributeSetBase> AttributeSetBase;

	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FECharacter")
	FText CharacterName;

	// 죽을 때 재생되는 애니메이션 몽타주
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	UAnimMontage* DeathMontage;

	// 캐릭터가 가지는 디폴트 어빌리티들. 캐릭터가 죽을 때 삭제되고, 부활할 때 다시 주어진다.
	// Default abilities for this Character. These will be removed on Character death and regiven if Character respawns.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<class UFEGameplayAbility>> CharacterAbilities;

	// 스폰, 리스폰 될 때 캐릭터의 초기화를 위한 디폴트 어트리뷰트들.
	// 스폰, 리스폰 될 때 재설정되는 어트리뷰트의 값을 재정의하는 인스턴스 GameEffect다.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	// 시작할 때만 적용되는 이펙트
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

	// 어빌리티를 서버에 저장한다. Ability Spec은 클라이언트에 리플리케이션된다. Grant abilities on the Server. The Ability Specs will be replicated to the owning client.
	virtual void AddCharacterAbilities();

	// 캐릭터의 어트리뷰트를 초기화한다. 서버에서 실행해야 하지만, 클라이언트에서도 실행된다.
	virtual void InitializeAttributes();

	virtual void AddStartupEffects();
	
	/**
	* 어트리뷰트들에 대한 Setter 함수들. Respawn과 같은 특수한 경우에만 사용, 그렇지 않은 경우 Gameplay Effect를 사용하여 어트리뷰트 변경
	* 어트리뷰트의 Base Value가 변경된다.
	*/

	virtual void SetHealth(float Health);
	virtual void SetEnergy(float Energy);
};
