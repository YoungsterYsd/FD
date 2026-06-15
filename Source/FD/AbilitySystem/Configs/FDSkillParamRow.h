// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Configs/FDTableMgrRowBase.h"
#include "FDSkillParamRow.generated.h"

/**
 * DataTable row for extra attribute bonus rule.
 *
 * Each row maps a RuleID to an attribute name, multiplier, and target type.
 * Linked from FDSkillInfoRow::ExtraValRuleID.
 */
USTRUCT()
struct FFDSkillParamRow : public FFDTableMgrRowBase
{
	GENERATED_BODY()

	/** Rule ID (links from FDSkillInfoRow::ExtraValRuleID). */
	UPROPERTY(EditAnywhere)
	int32 RuleID = 0;

	/** Attribute name for the extra bonus (e.g. "HP_Max"). */
	UPROPERTY(EditAnywhere)
	FName AttName;

	/** Multiplier applied to the attribute value. */
	UPROPERTY(EditAnywhere)
	float AttrMul = 0.0f;

	/** Target type ("Player" or "Enemy"). */
	UPROPERTY(EditAnywhere)
	FName TargetType;

	// ---- Query API ----

	static void Register();

	/** Find a param rule by RuleID. */
	static const FFDSkillParamRow* Find(int32 InRuleID);

private:
	static void EnsureCached();
	static TMap<int32, const FFDSkillParamRow*> ParamCache;
	static bool bCacheReady;
};
