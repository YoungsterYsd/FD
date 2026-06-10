// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "GameFramework/WorldSettings.h"
#include "FDWorldSettings.generated.h"

class UFDExperienceDefinition;

/**
 * 关卡级别的 WorldSettings，用于指定本关卡默认使用的 Experience。
 */
UCLASS()
class FD_API AFDWorldSettings : public AWorldSettings
{
    GENERATED_BODY()

public:
    /**
     * 本关卡默认使用的 Experience 数据资产。
     * 关卡加载时由 ExperienceManagerComponent 读取。
     */
    UPROPERTY(EditDefaultsOnly, Category = "Experience")
    TSoftObjectPtr<UFDExperienceDefinition> DefaultExperience;
};
