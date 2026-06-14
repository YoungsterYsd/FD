// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Attributes/FDAttributeSet.h"
#include "RPGTenacitySet.generated.h"

/**
 * FD RPG Tenacity (Stagger/Poise) AttributeSet.
 *
 * Tenacity acts as a secondary defensive bar. When depleted, the character may be staggered.
 * - TenacityCurrent is clamped to [0, TenacityMax].
 * - TenacityDamage is a Meta input pipe processed in PostGameplayEffectExecute.
 * - TenacityAtkRate amplifies tenacity damage dealt to enemies.
 * - BodyStrength provides passive tenacity damage reduction.
 * - OnTenacityZero is broadcast when TenacityCurrent reaches zero.
 */
UCLASS(BlueprintType)
class FD_API UFDRPGTenacitySet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UFDRPGTenacitySet();

	ATTRIBUTE_ACCESSORS(UFDRPGTenacitySet, TenacityCurrent);
	ATTRIBUTE_ACCESSORS(UFDRPGTenacitySet, TenacityMax);
	ATTRIBUTE_ACCESSORS(UFDRPGTenacitySet, TenacityAtkRate);
	ATTRIBUTE_ACCESSORS(UFDRPGTenacitySet, BodyStrength);
	ATTRIBUTE_ACCESSORS(UFDRPGTenacitySet, TenacityDamage);

	/** Broadcast when TenacityCurrent reaches zero after tenacity damage processing. */
	mutable FFDAttributeEvent OnTenacityZero;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UFUNCTION()
	virtual void OnRep_TenacityCurrent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_TenacityMax(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_TenacityAtkRate(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_BodyStrength(const FGameplayAttributeData& OldValue);

protected:
	/** Current tenacity, clamped to [0, TenacityMax]. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TenacityCurrent, Category = "FD|Tenacity")
	FGameplayAttributeData TenacityCurrent;

	/** Maximum tenacity capacity. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TenacityMax, Category = "FD|Tenacity")
	FGameplayAttributeData TenacityMax;

	/** Multiplier for tenacity damage dealt to enemies. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TenacityAtkRate, Category = "FD|Tenacity")
	FGameplayAttributeData TenacityAtkRate;

	/** Body strength for passive tenacity damage reduction. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BodyStrength, Category = "FD|Tenacity")
	FGameplayAttributeData BodyStrength;

	/**
	 * Tenacity damage input pipe (Meta attribute, not replicated).
	 * GameplayEffects add to this; PostGameplayEffectExecute subtracts it from TenacityCurrent then resets to zero.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "FD|Tenacity", Meta = (HideFromModifiers))
	FGameplayAttributeData TenacityDamage;
};
