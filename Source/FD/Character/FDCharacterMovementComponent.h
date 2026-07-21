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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FD|Movement")
    float SpeedMultiplier = 1.f;

public:
    virtual float GetMaxAcceleration() const override;
    virtual float GetMaxBrakingDeceleration() const override;
};
