// Copyright YoungSterYSD. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "FDEnergyCostExecution.generated.h"

/**
 * Reads DT_SkillEnergy via SetByCaller.SkillID/SkillLevel and consumes from UFDEnergySet.
 * Called by GE_Cost_Base during CommitAbility.
 * 
 * @note Requires UE_INLINE_GENERATED_CPP_BY_NAME in .cpp for proper UHT registration.
 */
UCLASS()
class FD_API UFDEnergyCostExecution : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()
public:
    UFDEnergyCostExecution();

    virtual void Execute_Implementation(
        const FGameplayEffectCustomExecutionParameters& ExecutionParams,
        FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
