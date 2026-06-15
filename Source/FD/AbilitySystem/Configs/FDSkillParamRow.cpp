// Copyright YoungSterYSD. All Rights Reserved.

#include "FDSkillParamRow.h"
#include "Engine/DataTable.h"

void FFDSkillParamRow::Register()
{
	FFDTableMgrRowBase::RegisterParser(TEXT("SkillParams"),
		FSoftObjectPath(TEXT("/Game/Data/DT_SkillParams")));
}

void FFDSkillParamRow::EnsureCached()
{
	if (bCacheReady)
	{
		return;
	}
	bCacheReady = true;

	UDataTable* Table = GetOrLoadTable(TEXT("SkillParams"));
	if (!Table)
	{
		return;
	}

	TArray<FFDSkillParamRow*> Rows = GetAllRows<FFDSkillParamRow>(Table);
	for (const FFDSkillParamRow* R : Rows)
	{
		ParamCache.Add(R->RuleID, R);
	}
}

const FFDSkillParamRow* FFDSkillParamRow::Find(int32 InRuleID)
{
	EnsureCached();
	const FFDSkillParamRow* const* Found = ParamCache.Find(InRuleID);
	return Found ? *Found : nullptr;
}

TMap<int32, const FFDSkillParamRow*> FFDSkillParamRow::ParamCache;
bool FFDSkillParamRow::bCacheReady = false;
