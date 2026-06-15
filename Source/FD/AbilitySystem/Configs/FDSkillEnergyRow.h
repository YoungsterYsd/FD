// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Configs/FDTableMgrRowBase.h"
#include "FDSkillEnergyRow.generated.h"

/**
 * DataTable row for skill energy costs and gains.
 *
 * Linked from DT_SkillInfo.EnergyRuleID.
 * RuleID groups all energy entries for a skill; SubID provides ordering within the group.
 * Direction is "Cost" or "Gain" to distinguish consumption from generation.
 */
USTRUCT()
struct FFDSkillEnergyRow : public FFDTableMgrRowBase
{
	GENERATED_BODY()

	/** Rule ID linking back to DT_SkillInfo.EnergyRuleID. */
	UPROPERTY(EditAnywhere)
	int32 RuleID = 0;

	/** Sub-identifier for ordering within the rule group. */
	UPROPERTY(EditAnywhere)
	int32 SubID = 0;

	/** "Cost" or "Gain". */
	UPROPERTY(EditAnywhere)
	FName Direction;

	/** Energy type tag (e.g. Energy.Type.General, Energy.Type.Ultimate). */
	UPROPERTY(EditAnywhere)
	FGameplayTag EnergyType;

	/** Amount of energy to consume or grant. */
	UPROPERTY(EditAnywhere)
	float Amount = 0.0f;

	// ---- Query API ----

	static void Register();

	/** Get all energy entries for a given rule ID, sorted by SubID. */
	static void GetEntriesByRule(int32 InRuleID, TArray<const FFDSkillEnergyRow*>& OutEntries);

private:
	static void EnsureCached();
	static TMap<int32, TArray<const FFDSkillEnergyRow*>> RuleCache;
	static bool bCacheReady;
};
