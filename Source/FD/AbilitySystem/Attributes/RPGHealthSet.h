// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Attributes/FDAttributeSet.h"
#include "RPGHealthSet.generated.h"

/**
 * FD RPG Health AttributeSet.
 *
 * Manages health-related attributes using a Meta Attribute pipeline:
 * - HpBasic / HpMul → HpMax is derived as HpBasic × (1 + HpMul).
 * - Healing and Damage are Meta input pipes processed in PostGameplayEffectExecute.
 * - HpCurrent is clamped to [0, HpMax].
 * - OnOutOfHealth is broadcast when HpCurrent reaches zero.
 */
UCLASS(BlueprintType)
class FD_API UFDRPGHealthSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UFDRPGHealthSet();

	ATTRIBUTE_ACCESSORS(UFDRPGHealthSet, HpBasic);
	ATTRIBUTE_ACCESSORS(UFDRPGHealthSet, HpMul);
	ATTRIBUTE_ACCESSORS(UFDRPGHealthSet, HpMax);
	ATTRIBUTE_ACCESSORS(UFDRPGHealthSet, HpCurrent);
	ATTRIBUTE_ACCESSORS(UFDRPGHealthSet, Healing);
	ATTRIBUTE_ACCESSORS(UFDRPGHealthSet, Damage);

	/** Broadcast when HpCurrent drops to or below zero after damage processing. */
	mutable FFDAttributeEvent OnOutOfHealth;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	UFUNCTION()
	virtual void OnRep_HpBasic(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_HpMul(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_HpMax(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_HpCurrent(const FGameplayAttributeData& OldValue);

private:
	/** Recalculate HpMax from HpBasic and HpMul, then clamp HpCurrent. */
	void RecalculateHpMax();

protected:
	/** Base health value (additive). */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HpBasic, Category = "FD|Health")
	FGameplayAttributeData HpBasic;

	/** Health multiplier (percentage, 0.0 = +0%). */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HpMul, Category = "FD|Health")
	FGameplayAttributeData HpMul;

	/** Derived max health: HpBasic × (1 + HpMul). Cannot be modified directly by GameplayEffects. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HpMax, Category = "FD|Health", Meta = (HideFromModifiers))
	FGameplayAttributeData HpMax;

	/** Current health, clamped to [0, HpMax]. Cannot be modified directly by GameplayEffects. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HpCurrent, Category = "FD|Health", Meta = (HideFromModifiers))
	FGameplayAttributeData HpCurrent;

	/**
	 * Healing input pipe (Meta attribute, not replicated).
	 * GameplayEffects add to this; PostGameplayEffectExecute transfers it to HpCurrent then resets to zero.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "FD|Health")
	FGameplayAttributeData Healing;

	/**
	 * Damage input pipe (Meta attribute, not replicated).
	 * GameplayEffects add to this; PostGameplayEffectExecute subtracts it from HpCurrent then resets to zero.
	 * Marked HideFromModifiers so only ExecutionCalculations can modify it.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "FD|Health", Meta = (HideFromModifiers))
	FGameplayAttributeData Damage;
};
