// Copyright YoungSterYSD. All Rights Reserved.

#include "FDSkillEnergyRow.h"
#include "Engine/DataTable.h"

void FFDSkillEnergyRow::Register()
{
	FFDTableMgrRowBase::RegisterParser(TEXT("SkillEnergy"),
		FSoftObjectPath(TEXT("/Game/Data/DT_SkillEnergy")));
}

void FFDSkillEnergyRow::EnsureCached()
{
	if (bCacheReady)
	{
		return;
	}
	bCacheReady = true;

	UDataTable* Table = GetOrLoadTable(TEXT("SkillEnergy"));
	if (!Table)
	{
		return;
	}

	TArray<FFDSkillEnergyRow*> Rows = GetAllRows<FFDSkillEnergyRow>(Table);
	for (const FFDSkillEnergyRow* R : Rows)
	{
		RuleCache.FindOrAdd(R->RuleID).Add(R);
	}

	// Sort each RuleID's entries by SubID
	for (auto& Pair : RuleCache)
	{
		Pair.Value.Sort([](const FFDSkillEnergyRow& A, const FFDSkillEnergyRow& B)
		{
			return A.SubID < B.SubID;
		});
	}
}

void FFDSkillEnergyRow::GetEntriesByRule(int32 InRuleID, TArray<const FFDSkillEnergyRow*>& OutEntries)
{
	EnsureCached();
	if (const TArray<const FFDSkillEnergyRow*>* Arr = RuleCache.Find(InRuleID))
	{
		OutEntries = *Arr;
	}
}

TMap<int32, TArray<const FFDSkillEnergyRow*>> FFDSkillEnergyRow::RuleCache;
bool FFDSkillEnergyRow::bCacheReady = false;
