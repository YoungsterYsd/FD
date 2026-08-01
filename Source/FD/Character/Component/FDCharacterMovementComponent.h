// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "FDCharacterMovementComponent.generated.h"

UCLASS()
class FD_API UFDCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:
    UFDCharacterMovementComponent();

    // ===== 通用移动 =====
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FD|Movement")
    float SpeedMultiplier = 1.f;

    // ===== 战斗朝向 =====

    /** Set a target to face during combat. Pass nullptr to clear. */
    void SetBattleFacingTarget(AActor* Target);

    /** Get the current battle facing target, or nullptr. */
    AActor* GetBattleFacingTarget() const { return BattleFacingTarget.Get(); }

    /** 检查是否被 GameplayTag 锁定移动（如技能蓄力期间） */
    bool IsMovementLocked() const;

public:
    virtual float GetMaxAcceleration() const override;
    virtual float GetMaxBrakingDeceleration() const override;

    /** 当移动锁定时拒绝非强制输入，由 CMC 自行拦截 */
    virtual void AddInputVector(FVector WorldVector, bool bForce) override;

    virtual void PhysicsRotation(float DeltaTime) override;

    virtual bool DoJump(bool bReplayingMoves, float DeltaTime) override;

private:
    /** Target actor to face when in combat. nullptr = use default movement-facing. */
    TWeakObjectPtr<AActor> BattleFacingTarget;
};
