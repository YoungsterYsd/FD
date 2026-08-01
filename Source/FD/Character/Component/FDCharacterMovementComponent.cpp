// Copyright YoungSterYSD. All Rights Reserved.

#include "Character/Component/FDCharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayTags/FDGameplayTags.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDCharacterMovementComponent)

UFDCharacterMovementComponent::UFDCharacterMovementComponent()
{
    // === 地面移动 ===
    RotationRate = FRotator(0.f, -1.f, 0.f);
    BrakingFrictionFactor = 1.f;
    BrakingFriction = 0.f;
    BrakingDecelerationWalking = 1500.f;
    GroundFriction = 5.f;
    MinAnalogWalkSpeed = 150.f;
    MaxWalkSpeed = 500.f;
    MaxAcceleration = 800.f;
    PerchRadiusThreshold = 20.0f;
    bUseFlatBaseForFloorChecks = true;
    bUseSeparateBrakingFriction = true;

    // === 跳跃 ===
    JumpZVelocity = 1500.f;
    AirControl = 0.3f;
    GravityScale = 3.0f;
    bNotifyApex = true;

    NavMovementProperties.bUseAccelerationForPaths = true;
}

float UFDCharacterMovementComponent::GetMaxAcceleration() const
{
    return Super::GetMaxAcceleration() * SpeedMultiplier;
}

float UFDCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
    return Super::GetMaxBrakingDeceleration() * SpeedMultiplier;
}

void UFDCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
    if (AActor* Target = BattleFacingTarget.Get())
    {
        // 战斗朝向：先设旋转，再调 Super 处理 root motion
        FVector ToTarget = FVector3d::ZeroVector - GetActorLocation();
        //Target->GetActorLocation() - GetActorLocation();
        ToTarget.Z = 0.f;
        if (!ToTarget.IsNearlyZero())
        {
            const float DesiredYaw = ToTarget.Rotation().Yaw;
            const float CurrentYaw = UpdatedComponent->GetComponentRotation().Yaw;
            const float Step = RotationRate.Yaw * DeltaTime;
            const float NewYaw = FMath::FixedTurn(CurrentYaw, DesiredYaw, Step);
            UpdatedComponent->SetWorldRotation(FRotator(0.f, NewYaw, 0.f));
        }

        // Super 处理 root motion（bOrient 临时屏蔽以避免干扰战斗朝向）
        const bool bSaved = bOrientRotationToMovement;
        bOrientRotationToMovement = false;
        Super::PhysicsRotation(DeltaTime);
        bOrientRotationToMovement = bSaved;
    }
    else
    {
        Super::PhysicsRotation(DeltaTime);
    }
}

void UFDCharacterMovementComponent::SetBattleFacingTarget(AActor* Target)
{
    BattleFacingTarget = Target;
}

bool UFDCharacterMovementComponent::DoJump(bool bReplayingMoves, float DeltaTime)
{
    return Super::DoJump(bReplayingMoves, DeltaTime);
}

bool UFDCharacterMovementComponent::IsMovementLocked() const
{
    const AActor* Owner = GetOwner();
    if (!Owner)
    {
        return false;
    }

    // Try owner's ASC first (NPC pattern: ASC on Character)
    if (const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner))
    {
        if (const UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
        {
            if (ASC->HasMatchingGameplayTag(FDGameplayTags::Status_MovementLocked))
            {
                return true;
            }
        }
    }

    // Try PlayerState's ASC (player pattern: ASC on PlayerState)
    if (const APawn* OwnerPawn = Cast<APawn>(Owner))
    {
        if (const APlayerState* PS = OwnerPawn->GetPlayerState())
        {
            if (const UAbilitySystemComponent* ASC = PS->FindComponentByClass<UAbilitySystemComponent>())
            {
                if (ASC->HasMatchingGameplayTag(FDGameplayTags::Status_MovementLocked))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void UFDCharacterMovementComponent::AddInputVector(FVector WorldVector, bool bForce)
{
    if (!IsMovementLocked() || bForce)
    {
        Super::AddInputVector(WorldVector, bForce);
    }
}
