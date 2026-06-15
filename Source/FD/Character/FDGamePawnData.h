// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "FDGamePawnData.generated.h"

class UFDAbilitySet;
class UFDInputConfig;
class UFDCameraMode;

/**
 * Character assembly data — describes what a Pawn is composed of.
 * GameMode reads this asset and spawns/initializes the character accordingly.
 */
UCLASS(BlueprintType, Const)
class FD_API UFDGamePawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** The Pawn class to instantiate. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn")
	TSubclassOf<APawn> PawnClass;

	/** Ability sets to grant to this character. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TObjectPtr<UFDAbilitySet>> AbilitySets;

	/** 输入配置 —— 定义 InputAction 到 GameplayTag 的映射 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UFDInputConfig> InputConfig;

	/** 默认相机模式（俯视角） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	TSubclassOf<UFDCameraMode> DefaultCameraMode;
};
