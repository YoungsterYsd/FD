// Copyright YoungSterYSD. All Rights Reserved.

#include "FDEnergyConfigRow.h"
#include "AbilitySystem/Attributes/FDEnergySet.h"
#include "Engine/DataTable.h"
#include "LogChannels/FDLogChannels.h"

void FFDEnergyConfigRow::ApplyTo(UFDEnergySet* Target)
{
	if (!Target)
	{
		return;
	}

	UDataTable* Table = FFDTableMgrRowBase::GetOrLoadTable(TEXT("EnergyConfig"));
	if (!Table)
	{
		return;
	}

	TArray<FFDEnergyConfigRow*> Rows = FFDTableMgrRowBase::GetAllRows<FFDEnergyConfigRow>(Table);

	for (const FFDEnergyConfigRow* Row : Rows)
	{
		if (Row->EnergyType.IsValid())
		{
			Target->RegisterEnergyPool(Row->EnergyType, Row->MaxCapacity, Row->ChargeRate);
		}
	}

	UE_LOG(LogFDGAS, Log,
		TEXT("FFDEnergyConfigRow::ApplyTo - Registered %d energy pools from DataTable."),
		Rows.Num());
}
