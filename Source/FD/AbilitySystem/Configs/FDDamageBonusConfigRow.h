// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "FDDamageBonusConfigRow.generated.h"

/**
 * DataTable row for global damage bonus type registration.
 *
 * Mirrors FDEnergyConfigRow: defines bonus types globally without per-character ID.
 * Used by UFDAttributeComponent::InitializeDamageBonuses to pre-populate
 * UFDRPGCombatSet::DamageBonuses with all known bonus types at initialization.
 *
 * Per-character differentiation is handled at runtime via equipment, skills,
 * and buffs — not through static config tables.
 */
USTRUCT()
struct FDDamageBonusConfigRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Damage bonus tag (e.g. Bonus.NormalAtk, Bonus.SpecialAtk, Bonus.ElementFire). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FD|DamageBonus")
	FGameplayTag BonusTag;

	/** Default bonus multiplier (0.0 = +0%, added to base damage multiplier). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FD|DamageBonus")
	float DefaultValue = 0.0f;
};
