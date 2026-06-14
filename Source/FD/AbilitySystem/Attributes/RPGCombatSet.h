// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Attributes/FDAttributeSet.h"
#include "RPGCombatSet.generated.h"

/**
 * FD RPG Combat AttributeSet.
 *
 * Manages offensive and defensive combat attributes.
 * - Atk is derived from AtkBasic × (1 + AtkMul) and cannot be modified directly.
 * - Crit is clamped to [0, 1].
 * - DmgInc/DmgDec control overall damage modifiers.
 * - BodyStrength provides passive tenacity/damage reduction.
 * - DamageBonuses is a tag-driven TMap for extensible skill-type damage bonuses.
 */
UCLASS(BlueprintType)
class FD_API UFDRPGCombatSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UFDRPGCombatSet();

	ATTRIBUTE_ACCESSORS(UFDRPGCombatSet, AtkBasic);
	ATTRIBUTE_ACCESSORS(UFDRPGCombatSet, AtkMul);
	ATTRIBUTE_ACCESSORS(UFDRPGCombatSet, Atk);
	ATTRIBUTE_ACCESSORS(UFDRPGCombatSet, Def);
	ATTRIBUTE_ACCESSORS(UFDRPGCombatSet, DefIgn);
	ATTRIBUTE_ACCESSORS(UFDRPGCombatSet, Crit);
	ATTRIBUTE_ACCESSORS(UFDRPGCombatSet, CritDmg);
	ATTRIBUTE_ACCESSORS(UFDRPGCombatSet, DmgInc);
	ATTRIBUTE_ACCESSORS(UFDRPGCombatSet, DmgDec);
	ATTRIBUTE_ACCESSORS(UFDRPGCombatSet, AttackSpeed);
	ATTRIBUTE_ACCESSORS(UFDRPGCombatSet, CastSpeed);
	ATTRIBUTE_ACCESSORS(UFDRPGCombatSet, BodyStrength);

	// ---- Tag-driven Damage Bonuses (extensible, no C++ changes for new types) ---- //

	/** Get the damage bonus multiplier for a given bonus tag. Returns 0.f if not found. */
	float GetDamageBonus(FGameplayTag BonusTag) const;

	/** Set the damage bonus multiplier for a given bonus tag. */
	void SetDamageBonus(FGameplayTag BonusTag, float Value);

	/** Check if a damage bonus tag exists. */
	bool HasDamageBonus(FGameplayTag BonusTag) const;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	UFUNCTION()
	virtual void OnRep_AtkBasic(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_AtkMul(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Atk(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Def(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_DefIgn(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Crit(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_CritDmg(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_DmgInc(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_DmgDec(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_AttackSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_CastSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_BodyStrength(const FGameplayAttributeData& OldValue);

private:
	/** Recalculate Atk from AtkBasic and AtkMul. */
	void RecalculateAtk();

protected:
	/** Base attack value (additive). */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AtkBasic, Category = "FD|Combat")
	FGameplayAttributeData AtkBasic;

	/** Attack multiplier (percentage, 0.0 = +0%). */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AtkMul, Category = "FD|Combat")
	FGameplayAttributeData AtkMul;

	/** Derived attack: AtkBasic × (1 + AtkMul). Cannot be modified directly by GameplayEffects. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Atk, Category = "FD|Combat", Meta = (HideFromModifiers))
	FGameplayAttributeData Atk;

	/** Defense value (flat damage reduction). */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Def, Category = "FD|Combat")
	FGameplayAttributeData Def;

	/** Defense penetration (ignores this amount of target's defense). */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DefIgn, Category = "FD|Combat")
	FGameplayAttributeData DefIgn;

	/** Critical hit rate, clamped to [0, 1]. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Crit, Category = "FD|Combat")
	FGameplayAttributeData Crit;

	/** Critical hit damage multiplier (e.g., 1.5 = +150%). */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritDmg, Category = "FD|Combat")
	FGameplayAttributeData CritDmg;

	/** Overall damage increase (positive percentage). */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DmgInc, Category = "FD|Combat")
	FGameplayAttributeData DmgInc;

	/** Overall damage decrease (negative percentage, applied to incoming damage). */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DmgDec, Category = "FD|Combat")
	FGameplayAttributeData DmgDec;

	/** Attack speed multiplier. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackSpeed, Category = "FD|Combat")
	FGameplayAttributeData AttackSpeed;

	/** Casting speed multiplier. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CastSpeed, Category = "FD|Combat")
	FGameplayAttributeData CastSpeed;

	/** Body strength for passive tenacity/damage reduction. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BodyStrength, Category = "FD|Combat")
	FGameplayAttributeData BodyStrength;

	/**
	 * Tag-driven damage bonuses (extensible, no C++ changes for new types).
	 * Key: Bonus.NormalAtk / Bonus.SpecialAtk / Bonus.ElementFire / etc.
	 * Value: multiplier (0.0~), added to base damage multiplier.
	 * @note Not replicated; initialized on server and synced via Server RPC.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "FD|Combat")
	TMap<FGameplayTag, float> DamageBonuses;
};
