// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Templates/Function.h"
#include "LogChannels/FDLogChannels.h"
#include "Config/Data/FFDSkillInfoData.h"
#include "Config/Data/FFDSkillParamData.h"
#include "Config/Data/FFDSkillEnergyData.h"

#include "UFDConfigSubsystem.generated.h"

/**
 * 配置中心子系统。
 * 使用 FInstancedStruct 提供类型安全的配置存储与查询能力，供 Lua 侧写入、C++ 侧读取。
 * 存储 Key 格式：StructPath_RowID（如 /Script/FD.FDHeroInitData_1001）。
 * StructPath（UScriptStruct 路径）是配置类型的唯一标识。
 */
UCLASS()
class FD_API UFDConfigSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// -------------------------------------------------------------------------
	// JSON Bridge Write API (BlueprintCallable, UnLua compatible)
	// -------------------------------------------------------------------------

	/**
	 * 通过 JSON 字符串写入配置行（UnLua 友好，避免 table→struct 转换警告）。
	 * Lua 侧 table → JSON 字符串 → C++ JsonObjectConverter → FInstancedStruct 存储。
	 * @param RowID      行 ID
	 * @param StructPath C++ 结构体路径（如 "/Script/FD.FDHeroInitData"）
	 * @param JsonData   JSON 格式的字段数据（如 '{"HpBasic":200,"HpMul":0.1}'）
	 */
	UFUNCTION(BlueprintCallable, Category = "FD|Config")
	void WriteConfigRowJson(int32 RowID, const FString& StructPath, const FString& JsonData);

	// -------------------------------------------------------------------------
	// Type-Safe Read API (C++ side)
	// -------------------------------------------------------------------------

	/**
	 * 类型安全读取配置行。用 T::StaticStruct() 推导 Key。
	 * @tparam T    - 目标 USTRUCT 类型
	 * @param RowID - 行 ID
	 * @return 数据指针，不存在或类型不匹配时返回 nullptr
	 */
	template<typename T>
	const T* GetConfigRow(int32 RowID) const
	{
		const FString KeyStr = FString::Printf(TEXT("%s_%d"),
			*T::StaticStruct()->GetPathName(), RowID);
		const FName Key = FName(*KeyStr);
		if (const FInstancedStruct* Found = ConfigStore.Find(Key))
		{
			const T* Data = Found->GetPtr<T>();
			if (Data)
			{
				UE_LOG(LogFDGAS, Log, TEXT("[FDConfig] GetConfigRow found - Key=%s Type=%s"), *Key.ToString(), *T::StaticStruct()->GetName());
			}
			else
			{
				UE_LOG(LogFDGAS, Warning, TEXT("[FDConfig] GetConfigRow type mismatch - Key=%s Expected=%s Actual=%s"),
					*Key.ToString(), *T::StaticStruct()->GetName(),
					Found->GetScriptStruct() ? *Found->GetScriptStruct()->GetName() : TEXT("null"));
			}
			return Data;
		}
		UE_LOG(LogFDGAS, Verbose, TEXT("[FDConfig] GetConfigRow not found - Key=%s"), *Key.ToString());
		return nullptr;
	}

	/**
	 * 遍历某结构体类型下所有配置行。
	 * FInstancedStruct 通过 GetPtr<T>() 提供内置类型安全访问。
	 * @tparam T       - 目标 USTRUCT 类型
	 * @param Callback  - 回调 (RowID, Data)，返回 false 时停止遍历
	 */
	template<typename T>
	void ForEachConfigRow(TFunctionRef<bool(int32 RowID, const T& Data)> Callback) const
	{
		const FString StructPrefix = T::StaticStruct()->GetPathName() + TEXT("_");
		UE_LOG(LogFDGAS, Log, TEXT("[FDConfig] ForEachConfigRow begin - Struct=%s"), *T::StaticStruct()->GetName());
		for (const auto& Pair : ConfigStore)
		{
			const FString KeyStr = Pair.Key.ToString();
			if (KeyStr.StartsWith(StructPrefix))
			{
				if (const T* Data = Pair.Value.GetPtr<T>())
				{
					UE_LOG(LogFDGAS, Verbose, TEXT("[FDConfig] ForEachConfigRow matched - Key=%s"), *KeyStr);
					const FString RowIDStr = KeyStr.RightChop(StructPrefix.Len());
					const int32 RowID = FCString::Atoi(*RowIDStr);
					if (!Callback(RowID, *Data))
					{
						break;
					}
				}
				else
				{
					UE_LOG(LogFDGAS, Warning, TEXT("[FDConfig] ForEachConfigRow type mismatch - Key=%s Expected=%s Actual=%s"),
						*KeyStr, *T::StaticStruct()->GetName(),
						Pair.Value.GetScriptStruct() ? *Pair.Value.GetScriptStruct()->GetName() : TEXT("null"));
				}
			}
		}
	}

	/** 检查是否存在指定配置行（按 StructPath + RowID 查询） */
	UFUNCTION(BlueprintCallable, Category = "FD|Config")
	bool HasConfigRow(const FString& StructPath, int32 RowID) const;

	/** 按 StructPath 检查是否存在配置行（C++ 模板推导，无需传路径） */
	template<typename T>
	bool HasConfigRowTyped(int32 RowID) const
	{
		return HasConfigRow(T::StaticStruct()->GetPathName(), RowID);
	}

	/** 获取指定 StructPath 下的配置行总数 */
	UFUNCTION(BlueprintCallable, Category = "FD|Config")
	int32 GetConfigRowCount(const FString& StructPath) const;

	/** 清除指定 StructPath 的所有配置行 */
	UFUNCTION(BlueprintCallable, Category = "FD|Config")
	void ClearConfigType(const FString& StructPath);

	/** 清空所有配置 */
	void ClearAllConfigs();

	/** 便捷获取 SubSystem 实例 */
	UFUNCTION(BlueprintCallable, Category = "Config", meta = (WorldContext = "WorldContextObject"))
	static UFDConfigSubsystem* Get(const UObject* WorldContextObject);

	/**
	 * 无需 WorldContext 的静态获取方法。
	 * 遍历 GEngine 的所有 WorldContext，返回第一个有效的 SubSystem 实例。
	 * 适用于静态缓存初始化等无法传入 WorldContext 的场景。
	 * @return SubSystem 实例，不存在时返回 nullptr
	 */
	static UFDConfigSubsystem* Get_Static();

private:
	/** 将 StructPath 和 RowID 拼接为存储 Key（格式：StructPath_RowID，如 /Script/FD.FDHeroInitData_1001） */
	static FName MakeKey(const FString& StructPath, int32 RowID);

	/** 类型安全的配置存储（FInstancedStruct 替代原 FTypedConfigStore 的 memcpy 方案） */
	UPROPERTY()
	TMap<FName, FInstancedStruct> ConfigStore;
};

// ============================================================================
// Generic cache templates — direct index over UFDConfigSubsystem data
// ============================================================================

/**
 * 单键缓存：按一个 Key 索引，提供 Find(Key) 查询。
 *
 * @tparam TData - 配置数据类型（USTRUCT）
 * @tparam TKey  - 索引键类型
 *
 * 示例: TFDConfigCache_SingleKey<FFDSkillParamData, int32> — 按 RuleID 查找
 */
template<typename TData, typename TKey>
struct TFDConfigCache_SingleKey
{
	/**
	 * 从 UFDConfigSubsystem 构建缓存索引。
	 * @param GetKeyFunc - 从 TData 提取索引键的回调
	 */
	void Build(TFunctionRef<TKey(const TData&)> GetKeyFunc)
	{
		if (bBuilt)
		{
			return;
		}
		bBuilt = true;

		UFDConfigSubsystem* Config = UFDConfigSubsystem::Get_Static();
		if (!Config)
		{
			return;
		}

		int32 Count = 0;
		Config->ForEachConfigRow<TData>([&Count](int32, const TData&) { ++Count; return true; });
		OwnedData.Reserve(Count);

		Config->ForEachConfigRow<TData>([&](int32, const TData& Data)
		{
			TData& Row = OwnedData.Add_GetRef(Data);
			Cache.Add(GetKeyFunc(Row), &Row);
			return true;
		});

		UE_LOG(LogFDGAS, Verbose, TEXT("TFDConfigCache_SingleKey<%s> - Loaded %d rows"),
			*FString(TData::StaticStruct()->GetName()), OwnedData.Num());
	}

	/** 按 Key 查找配置行，找不到返回 nullptr。 */
	const TData* Find(TKey Key) const
	{
		const TData* const* Found = Cache.Find(Key);
		return Found ? *Found : nullptr;
	}

private:
	TArray<TData> OwnedData;
	TMap<TKey, const TData*> Cache;
	bool bBuilt = false;
};

/**
 * 三键缓存：按 (K1, K2, K3) 三级索引，提供 Find(k1, k2, k3) 查询。
 *
 * @tparam TData - 配置数据类型（USTRUCT）
 * @tparam TKey1 - 一级索引键类型
 * @tparam TKey2 - 二级索引键类型
 * @tparam TKey3 - 三级索引键类型
 *
 * 示例: TFDConfigCache_CompositeKey<FFDSkillInfoData, int32, int32, int32>
 *       按 (SkillId, Level, Seg) 查找
 */
template<typename TData, typename TKey1, typename TKey2, typename TKey3>
struct TFDConfigCache_CompositeKey
{
	/**
	 * 从 UFDConfigSubsystem 构建三级缓存索引。
	 * @param GetKey1 - 从 TData 提取一级键的回调
	 * @param GetKey2 - 从 TData 提取二级键的回调
	 * @param GetKey3 - 从 TData 提取三级键的回调
	 */
	void Build(
		TFunctionRef<TKey1(const TData&)> GetKey1,
		TFunctionRef<TKey2(const TData&)> GetKey2,
		TFunctionRef<TKey3(const TData&)> GetKey3)
	{
		if (bBuilt)
		{
			return;
		}
		bBuilt = true;

		UFDConfigSubsystem* Config = UFDConfigSubsystem::Get_Static();
		if (!Config)
		{
			return;
		}

		int32 Count = 0;
		Config->ForEachConfigRow<TData>([&Count](int32, const TData&) { ++Count; return true; });
		OwnedData.Reserve(Count);

		Config->ForEachConfigRow<TData>([&](int32, const TData& Data)
		{
			TData& Row = OwnedData.Add_GetRef(Data);
			Cache.FindOrAdd(GetKey1(Row)).FindOrAdd(GetKey2(Row)).Add(GetKey3(Row), &Row);
			return true;
		});

		UE_LOG(LogFDGAS, Verbose, TEXT("TFDConfigCache_CompositeKey<%s> - Loaded %d rows"),
			*FString(TData::StaticStruct()->GetName()), OwnedData.Num());
	}

	/** 按三级键查找配置行，找不到返回 nullptr。 */
	const TData* Find(TKey1 K1, TKey2 K2, TKey3 K3) const
	{
		if (const auto* L2 = Cache.Find(K1))
		{
			if (const auto* L3 = L2->Find(K2))
			{
				if (const TData* const* Found = L3->Find(K3))
				{
					return *Found;
				}
			}
		}
		return nullptr;
	}

private:
	TArray<TData> OwnedData;
	TMap<TKey1, TMap<TKey2, TMap<TKey3, const TData*>>> Cache;
	bool bBuilt = false;
};

/**
 * 分组缓存：按 GroupKey 分组，每组按 SortKey 排序，提供 GetEntries(GroupKey, Out) 查询。
 *
 * @tparam TData     - 配置数据类型（USTRUCT）
 * @tparam TGroupKey - 分组键类型
 * @tparam TSortKey  - 组内排序键类型
 *
 * 示例: TFDConfigCache_Grouped<FFDSkillEnergyData, int32, int32>
 *       按 RuleID 分组，按 SubID 排序
 */
template<typename TData, typename TGroupKey, typename TSortKey>
struct TFDConfigCache_Grouped
{
	/**
	 * 从 UFDConfigSubsystem 构建分组缓存。
	 * @param GetGroupKey - 从 TData 提取分组键的回调
	 * @param GetSortKey  - 从 TData 提取排序键的回调
	 */
	void Build(
		TFunctionRef<TGroupKey(const TData&)> GetGroupKey,
		TFunctionRef<TSortKey(const TData&)> GetSortKey)
	{
		if (bBuilt)
		{
			return;
		}
		bBuilt = true;

		UFDConfigSubsystem* Config = UFDConfigSubsystem::Get_Static();
		if (!Config)
		{
			return;
		}

		int32 Count = 0;
		Config->ForEachConfigRow<TData>([&Count](int32, const TData&) { ++Count; return true; });
		OwnedData.Reserve(Count);

		Config->ForEachConfigRow<TData>([&](int32, const TData& Data)
		{
			TData& Row = OwnedData.Add_GetRef(Data);
			Cache.FindOrAdd(GetGroupKey(Row)).Add(&Row);
			return true;
		});

		// Sort each group by SortKey
		for (auto& Pair : Cache)
		{
			Pair.Value.Sort([&](const TData& A, const TData& B)
			{
				return GetSortKey(A) < GetSortKey(B);
			});
		}

		UE_LOG(LogFDGAS, Verbose, TEXT("TFDConfigCache_Grouped<%s> - Loaded %d rows, %d groups"),
			*FString(TData::StaticStruct()->GetName()), OwnedData.Num(), Cache.Num());
	}

	/** 获取指定分组的所有条目指针（已按 SortKey 排序）。 */
	void GetEntries(TGroupKey Key, TArray<const TData*>& Out) const
	{
		if (const TArray<const TData*>* Found = Cache.Find(Key))
		{
			Out = *Found;
		}
	}

private:
	TArray<TData> OwnedData;
	TMap<TGroupKey, TArray<const TData*>> Cache;
	bool bBuilt = false;
};

// ============================================================================
// Global skill cache instances (extern — defined in FDGameplayAbility.cpp)
// ============================================================================

/** Skill info cache: indexed by (SkillId, Level, Seg). */
extern TFDConfigCache_CompositeKey<FFDSkillInfoData, int32, int32, int32> GSkillInfoCache;

/** Skill param cache: indexed by RuleID. */
extern TFDConfigCache_SingleKey<FFDSkillParamData, int32> GSkillParamCache;

/** Skill energy cache: grouped by RuleID, sorted by SubID. */
extern TFDConfigCache_Grouped<FFDSkillEnergyData, int32, int32> GSkillEnergyCache;

/**
 * Initialize all skill config caches by reading from UFDConfigSubsystem.
 * Safe to call multiple times — each cache's Build() is guarded by bBuilt.
 * Must be called after UFDConfigSubsystem has been populated (after Lua config push).
 */
void InitializeSkillCaches();
