// Copyright YoungSterYSD. All Rights Reserved.

#include "FDCameraMode_TopDown.h"
#include "GameFramework/Actor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDCameraMode_TopDown)

void UFDCameraMode_TopDown::UpdateView(float DeltaTime, AActor* Target,
                                        FVector& OutLocation, FRotator& OutRotation, float& OutFOV)
{
    if (!Target)
        return;

    if (!bInitialized)
    {
        CurrentZoom = DefaultZoom;
        SmoothedPivot = Target->GetActorLocation() + PivotOffset;
        bInitialized = true;
    }

    // 目标位置 + 偏移
    const FVector DesiredPivot = Target->GetActorLocation() + PivotOffset;

    // 平滑插值
    SmoothedPivot = FMath::VInterpTo(SmoothedPivot, DesiredPivot, DeltaTime, LocationInterpSpeed);

    // 相机位置 = 焦点 - 前向量 * 距离
    OutLocation = SmoothedPivot - CameraAngle.Vector() * CurrentZoom;
    OutRotation = CameraAngle;
    OutFOV = 90.f;
}
