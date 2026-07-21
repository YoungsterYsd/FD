// Copyright YoungSterYSD. All Rights Reserved.

#include "Character/FDCharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDCharacterMovementComponent)

UFDCharacterMovementComponent::UFDCharacterMovementComponent()
{
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
