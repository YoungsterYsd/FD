// Copyright YoungSterYSD. All Rights Reserved.

#include "Animation/FDLocomotionAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UFDLocomotionAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 缓存 Character Owner，后续每帧通过此指针快速访问移动数据
	AActor* OwnerActor = GetOwningActor();
	if (OwnerActor)
	{
		CachedCharacter = Cast<ACharacter>(OwnerActor);
	}
}

void UFDLocomotionAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// Character 无效时跳过更新
	if (!CachedCharacter.IsValid())
	{
		return;
	}

	// --- 水平速度（俯视角，忽略 Z 轴） ---
	const FVector Velocity = CachedCharacter->GetVelocity();
	Speed = Velocity.Size2D();

	// --- 移动方向（相对于 Actor 朝向，-180 ~ 180 度） ---
	if (Speed > 10.0f)
	{
		const FVector Velocity2D = Velocity.GetSafeNormal2D();
		const FRotator ActorRotation = CachedCharacter->GetActorRotation();
		const FVector ForwardDir = ActorRotation.Vector();
		const FVector RightDir = FRotationMatrix(ActorRotation).GetScaledAxis(EAxis::Y);

		// 用 Dot(Forward) 和 Cross(Right) 求 Atan2，得到相对于角色朝向的角度
		const float ForwardDot = FVector::DotProduct(Velocity2D, ForwardDir);
		const float RightDot = FVector::DotProduct(Velocity2D, RightDir);

		// Atan2(Y, X)：Y=右侧分量，X=前方分量
		const float AngleRad = FMath::Atan2(RightDot, ForwardDot);
		Direction = FMath::RadiansToDegrees(AngleRad);
	}
	else
	{
		Direction = 0.0f;
	}

	// --- 是否在空中 ---
	if (UCharacterMovementComponent* MovementComp = CachedCharacter->GetCharacterMovement())
	{
		bIsInAir = MovementComp->IsFalling();
	}
	else
	{
		bIsInAir = false;
	}

	// --- 战斗姿态（预留，后续通过 GameplayTag 扩展） ---
	bIsCombatMode = false;

	// --- 死亡状态（预留，后续通过 HealthSet 扩展） ---
	bIsDead = false;
}
