// Copyright YoungSterYSD. All Rights Reserved.

#include "FDDamageBonusConfigRow.h"
#include "AbilitySystem/Attributes/FDCombatSet.h"
#include "Engine/DataTable.h"
#include "LogChannels/FDLogChannels.h"

void FFDDamageBonusConfigRow::ApplyTo(UFDCombatSet* Target)
{
	if (!Target)
	{
		return;
	}

	UDataTable* Table = FFDTableMgrRowBase::GetOrLoadTable(TEXT("DamageBonus"));
	if (!Table)
	{
		return;
	}

	TArray<FFDDamageBonusConfigRow*> Rows = FFDTableMgrRowBase::GetAllRows<FFDDamageBonusConfigRow>(Table);

	for (const FFDDamageBonusConfigRow* Row : Rows)
	{
		if (Row->BonusTag.IsValid())
		{
			Target->SetDamageBonus(Row->BonusTag, Row->DefaultValue);
		}
	}

	UE_LOG(LogFDGAS, Log,
		TEXT("FDDamageBonusConfigRow::ApplyTo - Registered %d damage bonus types from DataTable."),
		Rows.Num());
}
