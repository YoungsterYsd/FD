// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "FDExperienceDefinition.generated.h"

class UFDExperienceActionSet;

/**
 * 定义一次游戏体验（Experience）所需的所有数据。
 * 相当于一个"游戏模式数据配置"，驱动关卡加载、GameFeature 激活、
 * Action 执行等流程。
 */
UCLASS(BlueprintType, Const)
class FD_API UFDExperienceDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UFDExperienceDefinition();

    /**
     * 要激活的 GameFeature 插件 URL 列表。
     * 当前留空，后续阶段由 GameFeatureAction 机制填充。
     */
    UPROPERTY(EditDefaultsOnly, Category = "Experience")
    TArray<FString> GameFeaturesToEnable;

    /**
     * 本 Experience 要执行的 Action 列表。
     * 后续阶段由 GameFeatureAction 子类填充具体逻辑。
     */
    UPROPERTY(EditDefaultsOnly, Instanced, Category = "Experience")
    TArray<TObjectPtr<class UGameFeatureAction>> Actions;

    /**
     * 可复用的 Action 集合，允许多个 Experience 共享同一组 Action。
     */
    UPROPERTY(EditDefaultsOnly, Category = "Experience")
    TArray<TObjectPtr<UFDExperienceActionSet>> ActionSets;

#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
