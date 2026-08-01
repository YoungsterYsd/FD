// Copyright YoungSterYSD. All Rights Reserved.

#include "UFDAnimInstance.h"
#include "Character/FDCharacter.h"
#include "Character/Component/FDCharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTags/FDGameplayTags.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UFDAnimInstance)

void UFDAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    CachedCharacter = Cast<AFDCharacter>(TryGetPawnOwner());
    if (!CachedCharacter.IsValid()) { return; }

    CachedMovement = CachedCharacter->GetFDCharacterMovement();

    if (const AActor* Owner = GetOwningActor())
    {
        if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner))
        {
            InitializeWithAbilitySystem(ASC);
        }
    }
}

void UFDAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
    if (!ASC) { return; }
    GameplayTagPropertyMap.Initialize(this, ASC);
    UE_LOG(LogFD, Log, TEXT("[FDAnim] PropertyMap initialized with ASC"));
}

void UFDAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    RefreshLocomotionData(DeltaSeconds);
}

void UFDAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
    // Locomotion data refresh moved to NativeUpdateAnimation (Game Thread)
    // to avoid reading CMC data from Worker Thread mid-update.
}

void UFDAnimInstance::RefreshLocomotionData(float DeltaSeconds)
{
    if (!CachedCharacter.IsValid() || !CachedMovement.IsValid()) { return; }

    const FVector CurrentVelocity = CachedMovement->Velocity;
    Velocity = CurrentVelocity;
    Speed = UE_REAL_TO_FLOAT(CurrentVelocity.Size2D());
    VerticalSpeed = UE_REAL_TO_FLOAT(CurrentVelocity.Z);
    Direction = CalculateDirection(CurrentVelocity, CachedCharacter->GetActorRotation());

    const FVector CurrentAccel = CachedMovement->GetCurrentAcceleration();
    WorldAcceleration = CurrentAccel;
    const FQuat ActorQuat = CachedCharacter->GetActorQuat();
    Acceleration = ActorQuat.UnrotateVector(CurrentAccel);

    const float MaxAccel = CachedMovement->GetMaxAcceleration();
    LeanAmount = (MaxAccel > 0.0f) ? FMath::Clamp(Acceleration.Y / MaxAccel, -1.0f, 1.0f) : 0.0f;

    const FVector2D Prev2D(PreviousVelocity.X, PreviousVelocity.Y);
    const FVector2D Curr2D(CurrentVelocity.X, CurrentVelocity.Y);
    if (!Prev2D.IsNearlyZero() && !Curr2D.IsNearlyZero())
    {
        const float Dot = FVector2D::DotProduct(Prev2D, Curr2D);
        const float Cross = FVector2D::CrossProduct(Prev2D, Curr2D);
        TurnAngle = FMath::Abs(FMath::RadiansToDegrees(FMath::Atan2(Cross, Dot)));
    }
    else { TurnAngle = 0.0f; }
    PreviousVelocity = CurrentVelocity;

    bIsOnGround = CachedMovement->IsMovingOnGround();
    bHasMovementInput = !CachedMovement->GetPendingInputVector().IsNearlyZero();
    bPrevIsOnGround = bIsOnGround;
}

float UFDAnimInstance::CalculateDirection(const FVector& Vector, const FRotator& Rotation)
{
    if (Vector.IsNearlyZero() || Rotation.IsNearlyZero()) { return 0.f; }

    const float Length2D = FMath::Sqrt(Vector.X * Vector.X + Vector.Y * Vector.Y);
    if (Length2D < SMALL_NUMBER) { return 0.f; }

    const FVector2f NV{ UE_REAL_TO_FLOAT(Vector.X / Length2D), UE_REAL_TO_FLOAT(Vector.Y / Length2D) };
    const float YR = FMath::DegreesToRadians(Rotation.Yaw);
    const FVector2f FV{FMath::Cos(YR), FMath::Sin(YR)};
    const FVector2f RV{-FV.Y, FV.X};

    return FMath::UnwindDegrees(FMath::RadiansToDegrees(
        FMath::Atan2(NV.X * RV.X + NV.Y * RV.Y, NV.X * FV.X + NV.Y * FV.Y)));
}

FGameplayTag UFDAnimInstance::AngleToMoveDirectionTag(float Angle)
{
    const float AbsAngle = FMath::Abs(Angle);
    if (AbsAngle <= 22.5f) return FDGameplayTags::MoveDirection_F_0;
    if (AbsAngle >= 157.5f) return FDGameplayTags::MoveDirection_B_180;

    if (Angle > 0.f)
    {
        if (Angle <= 67.5f)  return FDGameplayTags::MoveDirection_F_R_45;
        if (Angle <= 112.5f) return FDGameplayTags::MoveDirection_R_90;
        return FDGameplayTags::MoveDirection_B_R_45;
    }
    if (Angle >= -67.5f)  return FDGameplayTags::MoveDirection_F_L_45;
    if (Angle >= -112.5f) return FDGameplayTags::MoveDirection_L_90;
    return FDGameplayTags::MoveDirection_B_L_45;
}

FGameplayTag UFDAnimInstance::GetLocomotionStateTag() const
{
    if (Speed < 10.0f)   return FDGameplayTags::CharState_Idle;
    if (Speed < WalkToRunSpeed)   return FDGameplayTags::CharState_Walk;
    if (Speed < RunToSprintSpeed) return FDGameplayTags::CharState_Run;
    return FDGameplayTags::CharState_Sprint;
}
