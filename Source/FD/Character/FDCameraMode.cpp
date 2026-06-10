// Copyright YoungSterYSD. All Rights Reserved.

#include "FDCameraMode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDCameraMode)

void UFDCameraMode::UpdateView(float DeltaTime, AActor* Target,
                                FVector& OutLocation, FRotator& OutRotation, float& OutFOV)
{
    // 基类默认实现：子类覆盖
}

void UFDCameraMode::AdjustZoom(float Delta)
{
    CurrentZoom = FMath::Clamp(CurrentZoom + Delta * 100.f, MinZoom, MaxZoom);
}
