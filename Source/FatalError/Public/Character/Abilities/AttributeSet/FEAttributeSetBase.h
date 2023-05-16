#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"		// 어빌리티 시스템 컴포넌트 헤더 파일 추가 필요.
#include "FEAttributeSetBase.generated.h"

// Attribute들에 대한 Getter와 Setter를 자동으로 생성해주는 매크로
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class FATALERROR_API UFEAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:
	UFEAttributeSetBase();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 현재 체력을 의미. 최대량이 MaxHealth 값으로 제한된다. 0이 되면 죽는다.
	// 체력 회복 관련 변화는 이 어트리뷰트를 직접 사용.
	// 체력 차감 관련 변화는 Damage 메타 어트리뷰트를 사용.
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UFEAttributeSetBase, Health)

	// 최대 체력을 의미. GameplayEffects에 의해 수정되는 어트리뷰트.
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UFEAttributeSetBase, MaxHealth)

	// 초당 체력 회복량을 의미. 
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_HealthRegenRate)
	FGameplayAttributeData HealthRegenRate;
	ATTRIBUTE_ACCESSORS(UFEAttributeSetBase, HealthRegenRate)

	// 현재 에너지를 의미. 최대량이 MaxEnergy 값으로 제한된다. 
	UPROPERTY(BlueprintReadOnly, Category = "Energy", ReplicatedUsing = OnRep_Energy)
	FGameplayAttributeData Energy;
	ATTRIBUTE_ACCESSORS(UFEAttributeSetBase, Energy)

	// 최대 에너지를 의미. GameplayEffects에 의해 수정되는 어트리뷰트.
	UPROPERTY(BlueprintReadOnly, Category = "Energy", ReplicatedUsing = OnRep_MaxEnergy)
	FGameplayAttributeData MaxEnergy;
	ATTRIBUTE_ACCESSORS(UFEAttributeSetBase, MaxEnergy)

	// 초당 에너지 회복량을 의미. 
	UPROPERTY(BlueprintReadOnly, Category = "Energy", ReplicatedUsing = OnRep_EnergyRegenRate)
	FGameplayAttributeData EnergyRegenRate;
	ATTRIBUTE_ACCESSORS(UFEAttributeSetBase, EnergyRegenRate)

	// 데미지를 줄여주는 어트리뷰트.
	UPROPERTY(BlueprintReadOnly, Category = "Armor", ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UFEAttributeSetBase, Armor)

	// Damage는 DamageExecution에서 최종 데미지를 계산하기 위해 사용되는 메타 어트리뷰트로, -Health로 변경된다.
	// 서버에만 존재하는 값으로, 리플레케이트 되지 않는다.
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UFEAttributeSetBase, Damage)

	// 캐릭터의 이동 속도를 의미.
	UPROPERTY(BlueprintReadOnly, Category = "MoveSpeed", ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UFEAttributeSetBase, MoveSpeed)

protected:
	// 최대 어트리뷰트가 변경될 때 이와 연관된 어트리뷰트의 값을 비례적으로 조정헤주는 함수
	// (MaxHealth가 증가하면 이전과 동일한 비율을 유지하는 만큼 Health도 증가한다.)
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

	/**
	 * OnRep 함수들은 레플리케이션 중 어빌리티 시스템의 내부 표현이 올바르게 동기화되도록 하기 위한 함수들이다.
	**/

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate);

	UFUNCTION()
	virtual void OnRep_Energy(const FGameplayAttributeData& OldEnergy);

	UFUNCTION()
	virtual void OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy);

	UFUNCTION()
	virtual void OnRep_EnergyRegenRate(const FGameplayAttributeData& OldEnergyRegenRate);

	UFUNCTION()
	virtual void OnRep_Armor(const FGameplayAttributeData& OldArmor);

	UFUNCTION()
	virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);
};