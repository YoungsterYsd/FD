// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Configs/FDTableMgrRowBase.h"
#include "FDDamageBonusConfigRow.generated.h"

class UFDCombatSet;

/**
 * DataTable row for global damage bonus type registration.
 *
 * Mirrors FDEnergyConfigRow: defines bonus types globally without per-character ID.
 * Used to pre-populate UFDCombatSet::DamageBonuses with all known bonus types.
 *
 * Per-character differentiation is handled at runtime via equipment, skills,
 * and buffs �?not through static config tables.
 */
USTRUCT()
struct FFDDamageBonusConfigRow : public FFDTableMgrRowBase
{
	GENERATED_BODY()

	/** Damage bonus tag (e.g. Bonus.NormalAtk, Bonus.SpecialAtk, Bonus.ElementFire). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FD|DamageBonus")
	FGameplayTag BonusTag;

	/** Default bonus multiplier (0.0 = +0%, added to base damage multiplier). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FD|DamageBonus")
	float DefaultValue = 0.0f;

	/** Register this parser with UFDTableMgr at module startup. */
	static void Register()
	{
		FFDTableMgrRowBase::RegisterParser(TEXT("DamageBonus"), FSoftObjectPath(TEXT("/Game/Data/DT_DamageBonusConfig")));
	}

	/** Read DT_DamageBonusConfig and register all damage bonus types on the given CombatSet. */
	static void ApplyTo(UFDCombatSet* Target);
};
