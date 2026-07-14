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
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "GameplayTags/FDGameplayTags.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

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
    UE_LOG(LogFDGAS, Log, TEXT("FDGameplayAbility::ActivateAbility - SkillID=%d, Level=%d"), SkillID, SkillLevel);

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        UE_LOG(LogFDGAS, Warning, TEXT("FDGameplayAbility::ActivateAbility - CommitAbility failed"));
        K2_EndAbility();
        return;
    }

    PlayAbilityMontage();

    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UFDGameplayAbility::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    if (bWasCancelled && AbilityAnimation.Montage)
    {
        StopAbilityMontage(0.1f);
    }

    // 移除动画状态 Tag
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        ASC->RemoveLooseGameplayTag(FDGameplayTags::Status_Animation_Playing);
        ASC->RemoveLooseGameplayTag(FDGameplayTags::Status_Animation_UpperBody);
        ASC->RemoveLooseGameplayTag(FDGameplayTags::Status_MovementLocked);
    }

    if (MontageEndedDelegateHandle.IsBound())
    {
        MontageEndedDelegateHandle.Unbind();
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// ============================================================================
//  动画方法
// ============================================================================

void UFDGameplayAbility::PlayAbilityMontage()
{
    if (!AbilityAnimation.Montage)
    {
        return;
    }

    UAnimInstance* AnimInst = GetAnimInstanceFromActorInfo();
    if (!AnimInst)
    {
        UE_LOG(LogFDGAS, Warning, TEXT("FDGameplayAbility::PlayAbilityMontage - No AnimInstance"));
        return;
    }

    AnimInst->Montage_Play(AbilityAnimation.Montage, AbilityAnimation.PlayRate,
        EMontagePlayReturnType::MontageLength, 0.0f, true);

    // 添加动画状态 Tag（GameplayTagPropertyMap 自动同步到 ABP 变量）
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        ASC->AddLooseGameplayTag(FDGameplayTags::Status_Animation_Playing);
        if (AbilityAnimation.bIsUpperBodySkill)
        {
            ASC->AddLooseGameplayTag(FDGameplayTags::Status_Animation_UpperBody);
        }
    }

    // 禁止移动时直接给 ASC 加 Status.MovementLocked Tag
    if (!AbilityAnimation.bCanMove)
    {
        if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
        {
            ASC->AddLooseGameplayTag(FDGameplayTags::Status_MovementLocked);
        }
    }

    FOnMontageEnded EndDelegate;
    EndDelegate.BindUObject(this, &UFDGameplayAbility::OnAbilityMontageEnded);
    AnimInst->Montage_SetEndDelegate(EndDelegate, AbilityAnimation.Montage);
    MontageEndedDelegateHandle = EndDelegate;

    UE_LOG(LogFDGAS, Log, TEXT("[FDAnim] SkillID=%d playing %s (bCanMove=%d, UpperBody=%d)"),
        SkillID, *AbilityAnimation.Montage->GetName(), AbilityAnimation.bCanMove, AbilityAnimation.bIsUpperBodySkill);
}

void UFDGameplayAbility::StopAbilityMontage(float BlendOutTime)
{
    if (!AbilityAnimation.Montage) return;

    if (UAnimInstance* AnimInst = GetAnimInstanceFromActorInfo())
    {
        AnimInst->Montage_Stop(BlendOutTime, AbilityAnimation.Montage);
    }
}

void UFDGameplayAbility::OnAbilityMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage != AbilityAnimation.Montage) return;

    MontageEndedDelegateHandle.Unbind();
    K2_EndAbility();
}

UAnimInstance* UFDGameplayAbility::GetAnimInstanceFromActorInfo() const
{
    if (const ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
    {
        return Character->GetMesh()->GetAnimInstance();
    }
    return nullptr;
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
    return GSkillInfoCache.Find(SkillID, SkillLevel, 1);
}
