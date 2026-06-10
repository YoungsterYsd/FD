// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "FDGamePawnData.generated.h"

class UFDAabilitySet;
class UFDInputConfig;
class UFDCameraMode;

/**
 * 角色组装数据 —— 描述一个 Pawn 由什么组成。
 * GameMode 读取此资产，按配置 Spawn 并初始化角色。
 */
UCLASS(BlueprintType, Const)
class FD_API UFDGamePawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** 要实例化的 Pawn 类 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn")
	TSubclassOf<APawn> PawnClass;

	/** 授予此角色的技能包 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TObjectPtr<UFDAabilitySet>> AbilitySets;

	/** 输入配置 —— 定义 InputAction 到 GameplayTag 的映射 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UFDInputConfig> InputConfig;

	/** 默认相机模式（俯视角） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	TSubclassOf<UFDCameraMode> DefaultCameraMode;
};
