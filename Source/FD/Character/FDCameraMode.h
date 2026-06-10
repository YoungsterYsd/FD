// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FDCameraMode.generated.h"

/**
 * 相机模式基类 —— 封装一种相机行为的参数与计算逻辑。
 * 子类只需实现 UpdateView() 和 AdjustZoom()。
 */
UCLASS(Blueprintable, BlueprintType)
class FD_API UFDCameraMode : public UObject
{
    GENERATED_BODY()

public:
    /** 计算本帧的相机 View 输出 */
    virtual void UpdateView(float DeltaTime, AActor* Target,
                            FVector& OutLocation, FRotator& OutRotation, float& OutFOV);

    /** 调整缩放值 */
    virtual void AdjustZoom(float Delta);

    /** 相机固定角度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
    FRotator CameraAngle = FRotator(-55.f, -45.f, 0.f);

    /** 缩放范围 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
    float MinZoom = 600.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
    float MaxZoom = 2200.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
    float DefaultZoom = 1200.f;

    /** 相机焦点偏移 —— 不在脚底而在目标上方 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
    FVector PivotOffset = FVector(0.f, 0.f, 80.f);

    /** 平滑跟随速度 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
    float LocationInterpSpeed = 10.f;

protected:
    float CurrentZoom;
    FVector SmoothedPivot;
    bool bInitialized = false;
};
