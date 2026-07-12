// Copyright YoungSterYSD. All Rights Reserved.

#include "Config/UFDConfigSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "LogChannels/FDLogChannels.h"
#include "JsonObjectConverter.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"

void UFDConfigSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogFDGAS, Log, TEXT("[FDConfig] Initialize"));
}

void UFDConfigSubsystem::Deinitialize()
{
	UE_LOG(LogFDGAS, Log, TEXT("[FDConfig] Deinitialize"));
	ClearAllConfigs();
	Super::Deinitialize();
}

// -------------------------------------------------------------------------
// JSON Bridge Write API
// -------------------------------------------------------------------------

void UFDConfigSubsystem::WriteConfigRowJson(int32 RowID, const FString& StructPath, const FString& JsonData)
{
	// 通过路径查找结构体类型
	UScriptStruct* StructType = FindObject<UScriptStruct>(nullptr, *StructPath);
	if (!StructType)
	{
		UE_LOG(LogFDGAS, Error, TEXT("[FDConfig] WriteConfigRowJson - StructType not found: %s"), *StructPath);
		return;
	}

	// Step 1: JSON 字符串 → FJsonObject
	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonData);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogFDGAS, Error, TEXT("[FDConfig] WriteConfigRowJson - JSON parse failed. StructPath=%s RowID=%d"),
			*StructPath, RowID);
		return;
	}

	// Step 2: FJsonObject → UStruct → FInstancedStruct
	FInstancedStruct Instanced;
	Instanced.InitializeAs(StructType);

	uint8* StructMemory = Instanced.GetMutableMemory();
	FMemory::Memzero(StructMemory, StructType->GetStructureSize());

	if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), StructType, StructMemory, 0, 0))
	{
		UE_LOG(LogFDGAS, Error, TEXT("[FDConfig] WriteConfigRowJson - UStruct deserialize failed. StructPath=%s RowID=%d"),
			*StructPath, RowID);
		return;
	}

	const FName Key = MakeKey(StructPath, RowID);
	ConfigStore.Add(Key, MoveTemp(Instanced));

	UE_LOG(LogFDGAS, Verbose, TEXT("[FDConfig] WriteConfigRowJson - Key=%s Type=%s"),
		*Key.ToString(), *StructType->GetName());
}

// -------------------------------------------------------------------------
// Query API
// -------------------------------------------------------------------------

bool UFDConfigSubsystem::HasConfigRow(const FString& StructPath, int32 RowID) const
{
	const FName Key = MakeKey(StructPath, RowID);
	const bool bFound = ConfigStore.Contains(Key);
	if (bFound)
	{
		UE_LOG(LogFDGAS, Log, TEXT("[FDConfig] HasConfigRow found - Key=%s"), *Key.ToString());
	}
	else
	{
		UE_LOG(LogFDGAS, Verbose, TEXT("[FDConfig] HasConfigRow not found - Key=%s"), *Key.ToString());
	}
	return bFound;
}

int32 UFDConfigSubsystem::GetConfigRowCount(const FString& StructPath) const
{
	const FString StructPrefix = StructPath + TEXT("_");
	int32 Count = 0;
	for (const auto& Pair : ConfigStore)
	{
		if (Pair.Key.ToString().StartsWith(StructPrefix))
		{
			++Count;
		}
	}
	UE_LOG(LogFDGAS, Log, TEXT("[FDConfig] GetConfigRowCount - StructPath=%s Count=%d"), *StructPath, Count);
	return Count;
}

void UFDConfigSubsystem::ClearConfigType(const FString& StructPath)
{
	const FString StructPrefix = StructPath + TEXT("_");
	TArray<FName> KeysToRemove;
	for (const auto& Pair : ConfigStore)
	{
		if (Pair.Key.ToString().StartsWith(StructPrefix))
		{
			KeysToRemove.Add(Pair.Key);
		}
	}

	for (const FName& Key : KeysToRemove)
	{
		ConfigStore.Remove(Key);
	}

	UE_LOG(LogFDGAS, Log, TEXT("[FDConfig] ClearConfigType - StructPath=%s Removed=%d"),
		*StructPath, KeysToRemove.Num());
}

void UFDConfigSubsystem::ClearAllConfigs()
{
	ConfigStore.Empty();
	UE_LOG(LogFDGAS, Log, TEXT("[FDConfig] ClearAllConfigs - All configs cleared"));
}

UFDConfigSubsystem* UFDConfigSubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("[FDConfig] Get failed - WorldContextObject is null"));
		return nullptr;
	}

	const UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("[FDConfig] Get failed - World is null"));
		return nullptr;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("[FDConfig] Get failed - GameInstance is null"));
		return nullptr;
	}

	return GameInstance->GetSubsystem<UFDConfigSubsystem>();
}

UFDConfigSubsystem* UFDConfigSubsystem::Get_Static()
{
	if (GEngine)
	{
		const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
		for (const FWorldContext& Context : WorldContexts)
		{
			if (UGameInstance* GI = Context.OwningGameInstance)
			{
				if (UFDConfigSubsystem* Subsys = GI->GetSubsystem<UFDConfigSubsystem>())
				{
					return Subsys;
				}
			}
		}
	}
	return nullptr;
}

FName UFDConfigSubsystem::MakeKey(const FString& StructPath, int32 RowID)
{
	const FString KeyStr = FString::Printf(TEXT("%s_%d"), *StructPath, RowID);
	return FName(*KeyStr);
}
