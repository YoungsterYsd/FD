// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FDTableMgr.generated.h"

class UDataTable;

USTRUCT()
struct FFDTableParserEntry
{
	GENERATED_BODY()

	FName Name;

	UPROPERTY()
	FSoftObjectPath TablePath;
};

UCLASS()
class FD_API UFDTableMgr : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void RegisterParser(const FName& Name, const FSoftObjectPath& Path);

	UDataTable* GetOrLoadTable(const FName& Name);

	bool IsTableLoaded(const FName& Name) const;

private:
	/** Registered parsers (name to path). */
	UPROPERTY()
	TMap<FName, FFDTableParserEntry> ParserRegistry;

	/** Loaded table cache. */
	UPROPERTY()
	TMap<FName, TObjectPtr<UDataTable>> LoadedTables;
};
