// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayEffectTypes.h"
#include "UFDAnimInstance.generated.h"

/** Cardinal + diagonal movement directions. */
UENUM(BlueprintType, meta = (ScriptName = "EFDMovementDirection"))
enum class EFDMovementDirection : uint8
{
    F  UMETA(DisplayName = "Forward"),
    FR UMETA(DisplayName = "ForwardRight"),
    RR UMETA(DisplayName = "Right"),
    BR UMETA(DisplayName = "BackwardRight"),
    B  UMETA(DisplayName = "Backward"),
    BL UMETA(DisplayName = "BackwardLeft"),
    LL UMETA(DisplayName = "Left"),
    FL UMETA(DisplayName = "ForwardLeft"),
};

class UAbilitySystemComponent;

/**
 * FD Animation Instance.
 *
 * C++ 层只暴露变量，蓝图层（ABP）读取变量做状态机决策。
 */
UCLASS()
class FD_API UFDAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadOnly, Category = "References", Transient)
    TWeakObjectPtr<class AFDCharacter> CachedCharacter;

    UPROPERTY(BlueprintReadOnly, Category = "References", Transient)
    TWeakObjectPtr<class UFDCharacterMovementComponent> CachedMovement;

public:
    // ===== ABP 可读的变量（每帧由 RefreshLocomotionData 更新） =====

    /** 水平速度 (cm/s) */
    UPROPERTY(BlueprintReadOnly, Category = "Locomotion", Transient)
    float Speed = 0.0f;

    /** 移动方向角度 [-180°, 180°] */
    UPROPERTY(BlueprintReadOnly, Category = "Locomotion", Transient)
    float Direction = 0.0f;

    /** Z 轴速度，正=上升 负=下落 */
    UPROPERTY(BlueprintReadOnly, Category = "Locomotion", Transient)
    float VerticalSpeed = 0.0f;

    /** 世界空间速度向量 */
    UPROPERTY(BlueprintReadOnly, Category = "Locomotion", Transient)
    FVector Velocity = FVector::ZeroVector;

    /** 局部空间加速度（角色前方=Y, 右侧=X） */
    UPROPERTY(BlueprintReadOnly, Category = "Locomotion", Transient)
    FVector Acceleration = FVector::ZeroVector;

    /** 加减速倾斜量 [-1,1]，正值=前倾加速，负值=后仰减速 */
    UPROPERTY(BlueprintReadOnly, Category = "Locomotion", Transient)
    float LeanAmount = 0.0f;

    /** 速度方向变化角 [0,180]°，0=直线 180=掉头 */
    UPROPERTY(BlueprintReadOnly, Category = "Locomotion", Transient)
    float TurnAngle = 0.0f;

    /** 是否在地面 (CMC::IsMovingOnGround) */
    UPROPERTY(BlueprintReadOnly, Category = "Locomotion", Transient)
    bool bIsOnGround = true;

    /** 世界空间加速度向量（用于点乘判定） */
    UPROPERTY(BlueprintReadOnly, Category = "Locomotion", Transient)
    FVector WorldAcceleration = FVector::ZeroVector;

    /** 是否有玩家移动输入 */
    UPROPERTY(BlueprintReadOnly, Category = "Locomotion", Transient)
    bool bHasMovementInput = false;

    /** Speed thresholds for gait transitions. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion|Gait")
    float WalkToRunSpeed = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion|Gait")
    float RunToSprintSpeed = 450.0f;

    // ===== 状态机 Transition 条件（BlueprintPure，供 ABP 直接使用） =====

    /** 是否有移动速度 */
    UFUNCTION(BlueprintPure, Category = "Locomotion|Transition", meta = (BlueprintThreadSafe))
    bool HasMovementSpeed() const { return Speed > 10.0f; }

    /** 是否有加速度 */
    UFUNCTION(BlueprintPure, Category = "Locomotion|Transition", meta = (BlueprintThreadSafe))
    bool HasAcceleration() const { return !Acceleration.IsNearlyZero(10.0f); }
	
    /** Move → Pivot: 有速度 && 有加速度 && 速度与加速度方向相反（减速/转向） */
    UFUNCTION(BlueprintPure, Category = "Locomotion|Transition", meta = (BlueprintThreadSafe))
    bool IsPivoting() const { return HasMovementSpeed() && HasAcceleration() && FVector::DotProduct(Velocity, WorldAcceleration) < 0.0f; }

    /** Ground → Jump: 在地面 && 上升速度超过阈值 */
    UFUNCTION(BlueprintPure, Category = "Locomotion|Transition", meta = (BlueprintThreadSafe))
    bool ShouldJump() const { return bIsOnGround && VerticalSpeed > 100.0f; }

    /** Jump → Fall: 已在空中 && 开始下落 */
    UFUNCTION(BlueprintPure, Category = "Locomotion|Transition", meta = (BlueprintThreadSafe))
    bool IsFalling() const { return !bIsOnGround && VerticalSpeed < -50.0f; }

    /** Air → Land: 刚接触地面（帧级边沿检测） */
    UFUNCTION(BlueprintPure, Category = "Locomotion|Transition", meta = (BlueprintThreadSafe))
    bool ShouldLand() const { return bIsOnGround && !bPrevIsOnGround; }

    /** Land → Move: 着陆且继续移动 */
    UFUNCTION(BlueprintPure, Category = "Locomotion|Transition", meta = (BlueprintThreadSafe))
    bool ShouldMoveLand() const { return ShouldLand() && Speed > 150.0f && bHasMovementInput; }

public:
    UFDAnimInstance() = default;

    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
    virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

    /** Called by FDAbilitySystemComponent when ASC is ready. */
    void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

    /** Get the custom movement component for direct access in ABP. */
    UFUNCTION(BlueprintPure, Category = "Locomotion", meta = (BlueprintThreadSafe))
    UFDCharacterMovementComponent* GetMovementComponent() const { return CachedMovement.Get(); }

    // ===== 动画资产 =====

    /** Animation lookup table set from Character or ABP defaults. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    TObjectPtr<class UFDAnimationSet> AnimationSet;

    /** Thread-safe getter for the animation lookup table. */
    UFUNCTION(BlueprintPure, Category = "Animation", meta = (BlueprintThreadSafe))
    UFDAnimationSet* GetAnimationSet() const;

private:
    /** Cached vel from previous frame for TurnAngle calculation */
    FVector PreviousVelocity = FVector::ZeroVector;

    /** Previous frame ground state for ShouldLand edge detection */
    bool bPrevIsOnGround = true;

    void RefreshLocomotionData(float DeltaSeconds);

    /** Calculate signed angle between velocity and actor forward direction. [-180, 180] degrees. */
    static float CalculateDirection(const FVector& Velocity, const FRotator& ActorRotation);

    /** Convert a direction angle to the nearest 8-direction MoveDirection tag. */
    UFUNCTION(BlueprintPure, Category = "Locomotion", meta = (BlueprintThreadSafe))
    static FGameplayTag AngleToMoveDirectionTag(float Angle);

    /** Map Speed to CharState tag (Idle/Walk/Run/Sprint) based on configurable thresholds. */
    UFUNCTION(BlueprintPure, Category = "Locomotion|Gait", meta = (BlueprintThreadSafe))
    FGameplayTag GetLocomotionStateTag() const;

    UPROPERTY(EditDefaultsOnly, Category = "FD|Animation")
    FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;
};
