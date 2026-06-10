// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "FDCameraMode.h"
#include "FDCameraMode_TopDown.generated.h"

/**
 * 俯视角相机模式 —— 固定斜45°跟随目标。
 */
UCLASS(Blueprintable)
class FD_API UFDCameraMode_TopDown : public UFDCameraMode
{
    GENERATED_BODY()

public:
    virtual void UpdateView(float DeltaTime, AActor* Target,
                            FVector& OutLocation, FRotator& OutRotation, float& OutFOV) override;
};
