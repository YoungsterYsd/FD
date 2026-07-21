// Copyright YoungSterYSD. All Rights Reserved.

#include "UFDAnimInstance.h"
#include "Character/FDCharacter.h"
#include "Character/FDCharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UFDAnimInstance)

void UFDAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    CachedCharacter = Cast<AFDCharacter>(TryGetPawnOwner());
    if (!CachedCharacter.IsValid())
    {
        return;
    }

    CachedMovement = Cast<UFDCharacterMovementComponent>(CachedCharacter->GetCharacterMovement());

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
    if (!ASC)
    {
        return;
    }
    GameplayTagPropertyMap.Initialize(this, ASC);
    UE_LOG(LogFD, Log, TEXT("[FDAnim] PropertyMap initialized with ASC"));
}

void UFDAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
    RefreshLocomotionData(DeltaSeconds);
}

void UFDAnimInstance::RefreshLocomotionData(float DeltaSeconds)
{
    if (!CachedCharacter.IsValid() || !CachedMovement.IsValid())
    {
        return;
    }

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
    if (MaxAccel > 0.0f)
    {
        LeanAmount = FMath::Clamp(Acceleration.Y / MaxAccel, -1.0f, 1.0f);
    }
    else
    {
        LeanAmount = 0.0f;
    }

    const FVector2D Prev2D(PreviousVelocity.X, PreviousVelocity.Y);
    const FVector2D Curr2D(CurrentVelocity.X, CurrentVelocity.Y);
    if (!Prev2D.IsNearlyZero() && !Curr2D.IsNearlyZero())
    {
        const float Dot = FVector2D::DotProduct(Prev2D, Curr2D);
        const float Cross = FVector2D::CrossProduct(Prev2D, Curr2D);
        TurnAngle = FMath::Abs(FMath::RadiansToDegrees(FMath::Atan2(Cross, Dot)));
    }
    else
    {
        TurnAngle = 0.0f;
    }
    PreviousVelocity = CurrentVelocity;

    bIsOnGround = CachedMovement->IsMovingOnGround();
    bHasMovementInput = !CachedMovement->GetPendingInputVector().IsNearlyZero();

    bPrevIsOnGround = bIsOnGround;
}

float UFDAnimInstance::CalculateDirection(const FVector& Vector, const FRotator& Rotation)
{
    if (Vector.IsNearlyZero() || Rotation.IsNearlyZero())
    {
        return 0.f;
    }

    const float Length2D = FMath::Sqrt(Vector.X * Vector.X + Vector.Y * Vector.Y);
    if (Length2D < SMALL_NUMBER)
    {
        return 0.f;
    }

    const FVector2f NormalizedVelocity{
        UE_REAL_TO_FLOAT(Vector.X / Length2D),
        UE_REAL_TO_FLOAT(Vector.Y / Length2D)
    };

    const float YawRad = FMath::DegreesToRadians(Rotation.Yaw);
    const float CosYaw = FMath::Cos(YawRad);
    const float SinYaw = FMath::Sin(YawRad);

    const FVector2f ForwardVector{CosYaw, SinYaw};
    const FVector2f RightVector{-SinYaw, CosYaw};

    const float ForwardDot = NormalizedVelocity.X * ForwardVector.X + NormalizedVelocity.Y * ForwardVector.Y;
    const float RightDot = NormalizedVelocity.X * RightVector.X + NormalizedVelocity.Y * RightVector.Y;

    return FMath::UnwindDegrees(FMath::RadiansToDegrees(FMath::Atan2(RightDot, ForwardDot)));
}
