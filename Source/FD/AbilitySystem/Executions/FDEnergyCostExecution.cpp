// Copyright YoungSterYSD. All Rights Reserved.
#include "FDEnergyCostExecution.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/FDEnergySet.h"
#include "AbilitySystem/Configs/FDSkillInfoRow.h"
#include "AbilitySystem/Configs/FDSkillEnergyRow.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDEnergyCostExecution)

UFDEnergyCostExecution::UFDEnergyCostExecution()
{
    UE_LOG(LogFDGAS, Log, TEXT("UFDEnergyCostExecution::UFDEnergyCostExecution - Constructor called"));
}

void UFDEnergyCostExecution::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

    const float SkillIDFloat = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.SkillID")), false, -1.0f);
    const float SkillLevelFloat = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.SkillLevel")), false, -1.0f);
    
    const int32 SkillID = FMath::RoundToInt(SkillIDFloat);
    const int32 SkillLevel = FMath::RoundToInt(SkillLevelFloat);

    UE_LOG(LogFDGAS, Verbose, TEXT("FDEnergyCostExecution::Execute - SkillID=%d, SkillLevel=%d"), SkillID, SkillLevel);

    if (SkillID <= 0)
    {
        UE_LOG(LogFDGAS, Warning, TEXT("FDEnergyCostExecution::Execute - Invalid SkillID, aborting"));
        return;
    }

    // Find the skill info (Seg=1 for cost, since cost is per-skill not per-segment)
    const FFDSkillInfoRow* Info = FFDSkillInfoRow::Find(SkillID, SkillLevel, 1);
    if (!Info)
    {
        UE_LOG(LogFDGAS, Warning, TEXT("FDEnergyCostExecution - SkillInfo not found for SkillID=%d, Level=%d"), SkillID, SkillLevel);
        return;
    }
    if (Info->EnergyRuleID == 0)
    {
        UE_LOG(LogFDGAS, Warning, TEXT("FDEnergyCostExecution - EnergyRuleID is 0 for SkillID=%d, Level=%d"), SkillID, SkillLevel);
        return;
    }

    // Get energy entries for this rule
    TArray<const FFDSkillEnergyRow*> Entries;
    FFDSkillEnergyRow::GetEntriesByRule(Info->EnergyRuleID, Entries);
    UE_LOG(LogFDGAS, Log, TEXT("FDEnergyCostExecution - RuleID=%d, Entries=%d"), Info->EnergyRuleID, Entries.Num());

    // Get ASC
    UAbilitySystemComponent* ASC = ExecutionParams.GetSourceAbilitySystemComponent();
    if (!ASC)
    {
        UE_LOG(LogFDGAS, Warning, TEXT("FDEnergyCostExecution - Source ASC is null"));
        return;
    }

    UFDEnergySet* ES = const_cast<UFDEnergySet*>(ASC->GetSet<UFDEnergySet>());
    if (!ES)
    {
        UE_LOG(LogFDGAS, Warning, TEXT("FDEnergyCostExecution - EnergySet not found on ASC"));
        return;
    }

    // Apply Cost entries (Direction == "Cost")
    for (const FFDSkillEnergyRow* Entry : Entries)
    {
        if (Entry->Direction != FName(TEXT("Cost")))
        {
            continue;
        }

        ES->ConsumeEnergy(Entry->EnergyType, Entry->Amount);
        UE_LOG(LogFDGAS, Log, TEXT("FDEnergyCostExecution - SkillID=%d Lv=%d: Consumed %.0f %s"),
            SkillID, SkillLevel, Entry->Amount, *Entry->EnergyType.ToString());
    }
}
