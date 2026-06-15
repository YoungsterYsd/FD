// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Configs/FDTableMgrRowBase.h"
#include "FDSkillInfoRow.generated.h"

/**
 * DataTable row for normalized skill definition (one row per segment).
 *
 * SkillId groups all segments and levels of the same skill.
 * Cooldown is defined on Seg=1 and shared across all segments.
 * Energy cost/gain is in the separate DT_SkillEnergy table.
 * ExtraValRuleID links to DT_SkillParams for attribute bonuses.
 */
USTRUCT()
struct FFDSkillInfoRow : public FFDTableMgrRowBase
{
	GENERATED_BODY()

	/** Display name for UI. */
	UPROPERTY(EditAnywhere)
	FName SkillName;

	/** Skill ID aggregating all segments and levels of the same skill. */
	UPROPERTY(EditAnywhere)
	int32 SkillId = 0;

	/** Damage segment number (1=first hit, 2=second, etc.). AN Tag determines which Seg hits. */
	UPROPERTY(EditAnywhere)
	int32 Seg = 1;

	/** Skill level (1-N). */
	UPROPERTY(EditAnywhere)
	int32 Level = 1;

	/** Attack multiplier for base damage formula: Atk × AtkMul + BasicVal. */
	UPROPERTY(EditAnywhere)
	float AtkMul = 1.0f;

	/** Fixed base damage value: Atk × AtkMul + BasicVal. */
	UPROPERTY(EditAnywhere)
	float BasicVal = 0.0f;

	/** Rule ID linking to DT_SkillParams for extra attribute bonus (0 = none). */
	UPROPERTY(EditAnywhere)
	int32 ExtraValRuleID = 0;

	/** Rule ID linking to DT_SkillEnergy for energy cost/gain (0 = none). */
	UPROPERTY(EditAnywhere)
	int32 EnergyRuleID = 0;

	/** Cooldown in seconds. Only meaningful on Seg=1 (shared by all segments). */
	UPROPERTY(EditAnywhere)
	float CooldownTime = 0.0f;

	// ---- Query API ----

	static void Register();

	/** Find a skill info row by SkillId, Level, Seg. */
	static const FFDSkillInfoRow* Find(int32 InSkillId, int32 InLevel, int32 InSeg);

	/** Get cooldown for a skill+level (always reads Seg=1). */
	static float GetCooldown(int32 InSkillId, int32 InLevel);

private:
	static void EnsureCached();
	static TMap<int32, TMap<int32, TMap<int32, const FFDSkillInfoRow*>>> SkillCache;
	static bool bCacheReady;
};
