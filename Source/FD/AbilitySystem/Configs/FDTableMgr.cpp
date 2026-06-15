// Copyright YoungSterYSD. All Rights Reserved.

#include "FDTableMgr.h"
#include "FDTableMgrBootstrap.h"
#include "Engine/DataTable.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDTableMgr)

void UFDTableMgr::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	FDTableMgr_RegisterAllParsers();
	
	// Pre-load all config tables so they're ready before first use
	for (auto& Pair : ParserRegistry)
	{
		GetOrLoadTable(Pair.Key);
	}
	
	UE_LOG(LogFDGAS, Log, TEXT("UFDTableMgr::Initialize - %d parsers registered, %d tables loaded."),
		ParserRegistry.Num(), LoadedTables.Num());
}

void UFDTableMgr::Deinitialize()
{
	ParserRegistry.Empty();
	LoadedTables.Empty();
	Super::Deinitialize();
}

void UFDTableMgr::RegisterParser(const FName& Name, const FSoftObjectPath& Path)
{
	if (ParserRegistry.Contains(Name))
	{
		UE_LOG(LogFDGAS, Warning, TEXT("UFDTableMgr::RegisterParser - Parser '%s' already registered."), *Name.ToString());
		return;
	}
	FFDTableParserEntry Entry;
	Entry.Name = Name;
	Entry.TablePath = Path;
	ParserRegistry.Add(Name, Entry);
	UE_LOG(LogFDGAS, Log, TEXT("UFDTableMgr::RegisterParser - Registered '%s' (%s)."), *Name.ToString(), *Path.ToString());
}

UDataTable* UFDTableMgr::GetOrLoadTable(const FName& Name)
{
	if (TObjectPtr<UDataTable>* Cached = LoadedTables.Find(Name))
	{
		return Cached->Get();
	}

	const FFDTableParserEntry* Entry = ParserRegistry.Find(Name);
	if (!Entry || Entry->TablePath.IsNull())
	{
		UE_LOG(LogFDGAS, Warning, TEXT("UFDTableMgr::GetOrLoadTable - Parser '%s' not found or path is null."), *Name.ToString());
		return nullptr;
	}

	UDataTable* Table = LoadObject<UDataTable>(nullptr, *Entry->TablePath.ToString());
	if (Table)
	{
		LoadedTables.Add(Name, Table);
		UE_LOG(LogFDGAS, Log, TEXT("UFDTableMgr::GetOrLoadTable - Loaded '%s'."), *Name.ToString());
	}
	return Table;
}

bool UFDTableMgr::IsTableLoaded(const FName& Name) const
{
	return LoadedTables.Contains(Name);
}
