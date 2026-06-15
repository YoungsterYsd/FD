// Copyright YoungSterYSD. All Rights Reserved.

#include "FDGameplayAbility.h"
#include "AbilitySystem/Configs/FDSkillInfoRow.h"
#include "AbilitySystem/Configs/FDSkillParamRow.h"
#include "AbilitySystem/Configs/FDSkillEnergyRow.h"
#include "AbilitySystem/Attributes/FDEnergySet.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDGameplayAbility)

UFDGameplayAbility::UFDGameplayAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

bool UFDGameplayAbility::CheckCost(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags))
    {
        return false;
    }

    // Check energy sufficiency from DT_SkillEnergy
    const FFDSkillInfoRow* Info = GetCurrentSkillInfo();
    if (Info && Info->EnergyRuleID != 0 && ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
    {
        const UFDEnergySet* ES = ActorInfo->AbilitySystemComponent->GetSet<UFDEnergySet>();
        if (ES)
        {
            TArray<const FFDSkillEnergyRow*> Entries;
            FFDSkillEnergyRow::GetEntriesByRule(Info->EnergyRuleID, Entries);
            for (const FFDSkillEnergyRow* Entry : Entries)
            {
                if (Entry->Direction == FName(TEXT("Cost")) &&
                    !ES->HasEnoughEnergy(Entry->EnergyType, Entry->Amount))
                {
                    UE_LOG(LogFDGAS, Warning, TEXT("FDGameplayAbility::CheckCost - Insufficient %s (have %.0f, need %.0f)"),
                        *Entry->EnergyType.ToString(), ES->GetEnergyCurrent(Entry->EnergyType), Entry->Amount);
                    return false;
                }
            }
        }
    }
    return true;
}

void UFDGameplayAbility::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    UE_LOG(LogFDGAS, Verbose, TEXT("FDGameplayAbility::ActivateAbility - SkillID=%d, Level=%d"), SkillID, SkillLevel);
    
    // CommitAbility internally handles: CheckCost → ApplyCost (calls our MakeOutgoingSpec with SetByCaller) → ApplyCooldown
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        UE_LOG(LogFDGAS, Warning, TEXT("FDGameplayAbility::ActivateAbility - CommitAbility failed"));
        K2_EndAbility();
        return;
    }
    
    UE_LOG(LogFDGAS, Log, TEXT("FDGameplayAbility::ActivateAbility - Committed, calling BP event"));
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    
    // End ability after BP event completes (C3: replaced by PlayMontageAndWait auto-end)
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

UGameplayEffect* UFDGameplayAbility::GetCostGameplayEffect() const
{
    if (CostGameplayEffectClass)
    {
        return CostGameplayEffectClass->GetDefaultObject<UGameplayEffect>();
    }
    return nullptr;
}

FGameplayEffectSpecHandle UFDGameplayAbility::MakeOutgoingGameplayEffectSpec(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    TSubclassOf<UGameplayEffect> GameplayEffectClass,
    float Level) const
{
    FGameplayEffectSpecHandle SpecHandle = Super::MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, GameplayEffectClass, Level);
    
    if (SpecHandle.IsValid())
    {
        if (GameplayEffectClass == CostGameplayEffectClass)
        {
            SpecHandle.Data->SetSetByCallerMagnitude(
                FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.SkillID")), 
                static_cast<float>(SkillID));
            SpecHandle.Data->SetSetByCallerMagnitude(
                FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.SkillLevel")), 
                static_cast<float>(SkillLevel));
        }
        else if (CooldownGameplayEffectClass && GameplayEffectClass == CooldownGameplayEffectClass)
        {
            const float CDTime = FFDSkillInfoRow::GetCooldown(SkillID, SkillLevel);
            if (CDTime > 0.0f)
            {
                SpecHandle.Data->SetDuration(CDTime, true);
                UE_LOG(LogFDGAS, Verbose, TEXT("FDGameplayAbility::MakeOutgoingSpec - CD=%.1fs"), CDTime);
            }
        }
    }
    
    return SpecHandle;
}

float UFDGameplayAbility::GetSkillParam(FName ParamKey) const
{
    const FFDSkillInfoRow* Info = GetCurrentSkillInfo();
    if (!Info || Info->ExtraValRuleID == 0) return 0.0f;
    const FFDSkillParamRow* Param = FFDSkillParamRow::Find(Info->ExtraValRuleID);
    if (Param && Param->AttName == ParamKey) return Param->AttrMul;
    return 0.0f;
}

const FFDSkillInfoRow* UFDGameplayAbility::GetCurrentSkillInfo() const
{
    return FFDSkillInfoRow::Find(SkillID, SkillLevel, CurrentSeg);
}
