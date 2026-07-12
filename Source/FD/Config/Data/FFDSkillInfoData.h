// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FFDSkillInfoData.generated.h"

USTRUCT(BlueprintType)
struct FFDSkillInfoData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Skill")
    FName SkillName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Skill")
    int32 SkillId = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Skill")
    int32 Seg = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Skill")
    int32 Level = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Skill")
    float AtkMul = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Skill")
    float BasicVal = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Skill")
    int32 ExtraValRuleID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Skill")
    int32 EnergyRuleID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Skill")
    float CooldownTime = 0.0f;
};
