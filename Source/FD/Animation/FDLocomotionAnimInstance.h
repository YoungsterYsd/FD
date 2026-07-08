// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FDLocomotionAnimInstance.generated.h"

class ACharacter;
class UCharacterMovementComponent;

/**
 * FD 项目角色移动动画实例基类
 * 驱动角色的移动动画状态（Idle/Walk/Run/Jump），供动画蓝图读取。
 * 基于俯视角游戏设计，水平速度忽略 Z 轴。
 */
UCLASS()
class FD_API UFDLocomotionAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// ~UAnimInstance interface
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	// ~UAnimInstance interface

protected:
	/** 水平移动速度（俯视角，仅 X/Y 平面，忽略 Z 轴） */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	float Speed = 0.0f;

	/** 移动方向相对于 Actor 朝向的角度（-180 ~ 180 度），0 表示正前方 */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	float Direction = 0.0f;

	/** 是否处于空中（跳跃/下落） */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	bool bIsInAir = false;

	/** 是否处于战斗姿态（预留，后续通过 GameplayTag 扩展） */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	bool bIsCombatMode = false;

	/** 是否已死亡（预留，后续通过 HealthSet 扩展） */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	bool bIsDead = false;

	/** 缓存的 Character Owner，用于快速访问移动组件 */
	UPROPERTY(Transient)
	TWeakObjectPtr<ACharacter> CachedCharacter;
};
