// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FFDSkillParamData.generated.h"

USTRUCT(BlueprintType)
struct FFDSkillParamData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Skill")
    int32 RuleID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Skill")
    FName AttName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Skill")
    float AttrMul = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Skill")
    FName TargetType;
};
