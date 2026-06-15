// Copyright YoungSterYSD. All Rights Reserved.

#include "FDSkillInfoRow.h"
#include "Engine/DataTable.h"
#include "LogChannels/FDLogChannels.h"

void FFDSkillInfoRow::Register()
{
	FFDTableMgrRowBase::RegisterParser(TEXT("SkillInfo"),
		FSoftObjectPath(TEXT("/Game/Data/DT_SkillInfo")));
}

void FFDSkillInfoRow::EnsureCached()
{
	if (bCacheReady)
	{
		return;
	}
	bCacheReady = true;

	UDataTable* Table = GetOrLoadTable(TEXT("SkillInfo"));
	if (!Table)
	{
		return;
	}

	TArray<FFDSkillInfoRow*> Rows = GetAllRows<FFDSkillInfoRow>(Table);
	UE_LOG(LogFDGAS, Verbose, TEXT("FFDSkillInfoRow::EnsureCached - Loaded %d rows"), Rows.Num());

	for (const FFDSkillInfoRow* R : Rows)
	{
		SkillCache.FindOrAdd(R->SkillId).FindOrAdd(R->Level).Add(R->Seg, R);
	}
}

const FFDSkillInfoRow* FFDSkillInfoRow::Find(int32 InSkillId, int32 InLevel, int32 InSeg)
{
	EnsureCached();
	if (const TMap<int32, TMap<int32, const FFDSkillInfoRow*>>* LevelMap = SkillCache.Find(InSkillId))
	{
		if (const TMap<int32, const FFDSkillInfoRow*>* SegMap = LevelMap->Find(InLevel))
		{
			if (const FFDSkillInfoRow* const* Row = SegMap->Find(InSeg))
			{
				return *Row;
			}
		}
	}
	return nullptr;
}

float FFDSkillInfoRow::GetCooldown(int32 InSkillId, int32 InLevel)
{
	const FFDSkillInfoRow* Row = Find(InSkillId, InLevel, 1);
	return Row ? Row->CooldownTime : 0.0f;
}

TMap<int32, TMap<int32, TMap<int32, const FFDSkillInfoRow*>>> FFDSkillInfoRow::SkillCache;
bool FFDSkillInfoRow::bCacheReady = false;
