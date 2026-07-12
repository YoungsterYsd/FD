// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FFDSkillEnergyData.generated.h"

USTRUCT(BlueprintType)
struct FFDSkillEnergyData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Skill")
    int32 RuleID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Skill")
    int32 SubID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Skill")
    FName Direction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Skill")
    FGameplayTag EnergyType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Skill")
    float Amount = 0.0f;
};
