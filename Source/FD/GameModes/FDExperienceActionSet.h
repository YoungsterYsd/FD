// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "FDExperienceActionSet.generated.h"

/**
 * 可复用的 GameFeatureAction 集合。
 * 多个 ExperienceDefinition 可以引用同一个 ActionSet，
 * 避免重复配置相同的 Action 列表。
 */
UCLASS(BlueprintType, Const)
class FD_API UFDExperienceActionSet : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UFDExperienceActionSet();

    /**
     * 本集合包含的 GameFeatureAction 列表。
     */
    UPROPERTY(EditDefaultsOnly, Instanced, Category = "Actions")
    TArray<TObjectPtr<class UGameFeatureAction>> Actions;

#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
