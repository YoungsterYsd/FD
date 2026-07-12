// Copyright YoungSterYSD. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FDGameplayAbility.generated.h"

struct FFDSkillInfoData;
class UGameplayEffect;

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

    /** Current damage segment, set by AnimNotify before damage calc. */
    UPROPERTY(BlueprintReadWrite, Category = "FD|Skill")
    int32 CurrentSeg = 1;

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
};
