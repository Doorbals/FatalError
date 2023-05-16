// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "FEPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class FATALERROR_API AFEPlayerState : public APlayerState, public IAbilitySystemInterface	// IAbilitySystemInterface 상속받음.
{
	GENERATED_BODY()

public:
	AFEPlayerState();

	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UFEAttributeSetBase* GetAttributeSetBase();
	
	UFUNCTION(BlueprintCallable, Category = "FEPlayerState")
	bool IsAlive();

	/**
	* FEAttributeSetBase의 어트리뷰트들에 대한 Getter. 따로 지정되지 않은 경우 Current Value 반환
	*/

	UFUNCTION(BlueprintCallable, Category = "FEPlayerState|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "FEPlayerState|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "FEPlayerState|Attributes")
	float GetHealthRegenRate() const;

	UFUNCTION(BlueprintCallable, Category = "FEPlayerState|Attributes")
	float GetEnergy() const;

	UFUNCTION(BlueprintCallable, Category = "FEPlayerState|Attributes")
	float GetMaxEnergy() const;

	UFUNCTION(BlueprintCallable, Category = "FEPlayerState|Attributes")
	float GetEnergyRegenRate() const;

	UFUNCTION(BlueprintCallable, Category = "FEPlayerState|Attributes")
	float GetArmor() const;

	UFUNCTION(BlueprintCallable, Category = "FEPlayerState|Attributes")
	float GetMoveSpeed() const;

protected:
	UPROPERTY()
	class UFEAbilitySystemComponent* AbilitySystemComponent;

	// 
	UPROPERTY()
	class UFEAttributeSetBase* AttributeSetBase;

	FGameplayTag DeadTag;

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle HealthRegenRateChangedDelegateHandle;
	FDelegateHandle EnergyChangedDelegateHandle;
	FDelegateHandle MaxEnergyChangedDelegateHandle;
	FDelegateHandle EnergyRegenRateChangedDelegateHandle;
	
	virtual void BeginPlay() override;

	// 어트리뷰트가 변경될 때 호출되는 콜백 함수들
	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void HealthRegenRateChanged(const FOnAttributeChangeData& Data);
	virtual void EnergyChanged(const FOnAttributeChangeData& Data);
	virtual void MaxEnergyChanged(const FOnAttributeChangeData& Data);
	virtual void EnergyRegenRateChanged(const FOnAttributeChangeData& Data);
	
	// 태그가 변경될 때 호출되는 콜백 함수
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
};
