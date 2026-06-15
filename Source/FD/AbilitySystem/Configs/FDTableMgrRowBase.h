// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FDTableMgrRowBase.generated.h"

class UDataTable;
class UFDTableMgr;

/**
 * Base class for all FD config DataTable rows.
 * Provides common TableMgr interaction utilities.
 * All new config tables should inherit from this struct.
 */
USTRUCT()
struct FFDTableMgrRowBase : public FTableRowBase
{
	GENERATED_BODY()

	/** Register a parser with UFDTableMgr (call once per row type at startup). */
	static void RegisterParser(const FName& Name, const FSoftObjectPath& Path);

	/** Get or lazy-load a DataTable by parser name. */
	static UDataTable* GetOrLoadTable(const FName& Name);

	/** Get all typed rows from a table. */
	template<typename TRow>
	static TArray<TRow*> GetAllRows(UDataTable* Table)
	{
		TArray<TRow*> Rows;
		if (Table)
		{
			Table->GetAllRows<TRow>(TEXT("Parse"), Rows);
		}
		return Rows;
	}

private:
	static UFDTableMgr* GetTableMgr();
};
