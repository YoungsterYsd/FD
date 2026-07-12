// Copyright YoungSterYSD. All Rights Reserved.

#include "FDGameplayAbility.h"
#include "Config/Data/FFDSkillInfoData.h"
#include "Config/Data/FFDSkillParamData.h"
#include "Config/Data/FFDSkillEnergyData.h"
#include "Config/UFDConfigSubsystem.h"
#include "AbilitySystem/Attributes/FDEnergySet.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDGameplayAbility)

// ============================================================================
// Global cache instances — defined here per FDConfigCache.h extern declarations
// ============================================================================

TFDConfigCache_CompositeKey<FFDSkillInfoData, int32, int32, int32> GSkillInfoCache;
TFDConfigCache_SingleKey<FFDSkillParamData, int32> GSkillParamCache;
TFDConfigCache_Grouped<FFDSkillEnergyData, int32, int32> GSkillEnergyCache;

void InitializeSkillCaches()
{
	GSkillInfoCache.Build(
		[](const FFDSkillInfoData& D) -> int32 { return D.SkillId; },
		[](const FFDSkillInfoData& D) -> int32 { return D.Level; },
		[](const FFDSkillInfoData& D) -> int32 { return D.Seg; });

	GSkillParamCache.Build(
		[](const FFDSkillParamData& D) -> int32 { return D.RuleID; });

	GSkillEnergyCache.Build(
		[](const FFDSkillEnergyData& D) -> int32 { return D.RuleID; },
		[](const FFDSkillEnergyData& D) -> int32 { return D.SubID; });
}

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

    InitializeSkillCaches();

    // Check energy sufficiency from DT_SkillEnergy
    const FFDSkillInfoData* Info = GetCurrentSkillInfo();
    if (Info && Info->EnergyRuleID != 0 && ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
    {
        const UFDEnergySet* ES = ActorInfo->AbilitySystemComponent->GetSet<UFDEnergySet>();
        if (ES)
        {
            TArray<const FFDSkillEnergyData*> Entries;
            GSkillEnergyCache.GetEntries(Info->EnergyRuleID, Entries);
            for (const FFDSkillEnergyData* Entry : Entries)
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
    
    // Note: EndAbility is no longer called here.
    // GA lifecycle is now controlled by AN_EndAbility on the montage or montage completion.
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
            InitializeSkillCaches();
            const FFDSkillInfoData* Info = GSkillInfoCache.Find(SkillID, SkillLevel, 1);
            const float CDTime = Info ? Info->CooldownTime : 0.0f;
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
    InitializeSkillCaches();
    const FFDSkillInfoData* Info = GetCurrentSkillInfo();
    if (!Info || Info->ExtraValRuleID == 0) return 0.0f;
    const FFDSkillParamData* Param = GSkillParamCache.Find(Info->ExtraValRuleID);
    if (Param && Param->AttName == ParamKey) return Param->AttrMul;
    return 0.0f;
}

const FFDSkillInfoData* UFDGameplayAbility::GetCurrentSkillInfo() const
{
    InitializeSkillCaches();
    return GSkillInfoCache.Find(SkillID, SkillLevel, CurrentSeg);
}
