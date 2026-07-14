// Copyright YoungSterYSD. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FDGameplayAbility.generated.h"

struct FFDSkillInfoData;
class UGameplayEffect;
class UAnimMontage;
class UAnimInstance;

/**
 * 技能动画配置数据结构。
 */
USTRUCT(BlueprintType)
struct FFDAbilityAnimation
{
    GENERATED_BODY()

    /** 技能 Montage，为空 = 无动画 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    TObjectPtr<UAnimMontage> Montage;

    /** 播放速率 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    float PlayRate = 1.0f;

    /** true = 上半身混合（下身继续移动）, false = 全身覆盖 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    bool bIsUpperBodySkill = false;

    /** 释放时是否允许移动（false 时 GA 播放期间直接加 Status.MovementLocked Tag） */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    bool bCanMove = false;
};

UCLASS(Blueprintable)
class FD_API UFDGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()
public:
    UFDGameplayAbility();

    /** Skill ID linking to DT_SkillInfo (SkillId column). */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FD|Skill")
    int32 SkillID = 0;

    /** Current skill level (1-N). */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FD|Skill")
    int32 SkillLevel = 1;

    /** 该技能对应的动画配置 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FD|Skill|Animation")
    FFDAbilityAnimation AbilityAnimation;

    /** Get extra attribute bonus from DT_SkillParams. */
    UFUNCTION(BlueprintCallable, Category = "FD|Skill")
    float GetSkillParam(FName ParamKey) const;

    /** Get current skill info data from config cache. */
    const FFDSkillInfoData* GetCurrentSkillInfo() const;

protected:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled) override;

    virtual bool CheckCost(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        FGameplayTagContainer* OptionalRelevantTags) const override;

    virtual UGameplayEffect* GetCostGameplayEffect() const override;

    virtual FGameplayEffectSpecHandle MakeOutgoingGameplayEffectSpec(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        TSubclassOf<UGameplayEffect> GameplayEffectClass,
        float Level = 1.0f) const override;

    // ---- 动画 ----

    void PlayAbilityMontage();
    void StopAbilityMontage(float BlendOutTime = 0.2f);

    UFUNCTION()
    void OnAbilityMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    UAnimInstance* GetAnimInstanceFromActorInfo() const;

    FOnMontageEnded MontageEndedDelegateHandle;
};
