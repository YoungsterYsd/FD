// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "FDGameCameraComponent.generated.h"

class UFDCameraMode;
struct FInputActionValue;

/**
 * 相机调度器 —— 管理 ModeStack。
 * 推入新模式时自动切换，弹出时恢复上一模式。
 */
UCLASS()
class FD_API UFDGameCameraComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFDGameCameraComponent();

    virtual void BeginPlay() override;

    /** 推入新相机模式 */
    UFUNCTION(BlueprintCallable)
    void PushCameraMode(TSubclassOf<UFDCameraMode> ModeClass);

    /** 弹出当前相机模式，恢复上一层 */
    UFUNCTION(BlueprintCallable)
    void PopCameraMode();

    /** 获取当前活跃模式 */
    UFUNCTION(BlueprintPure)
    UFDCameraMode* GetActiveCameraMode() const { return ActiveMode; }

    /** 滚轮缩放 —— 转发给当前模式 */
    void AdjustZoom(const struct FInputActionValue& Value);

    /** 激活默认俯视角相机 */
    void ActivateDefaultCamera();

    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    /** 默认相机模式类 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    TSubclassOf<UFDCameraMode> DefaultModeClass;

private:
    UPROPERTY()
    TObjectPtr<UFDCameraMode> ActiveMode;

    UPROPERTY()
    TArray<TObjectPtr<UFDCameraMode>> ModeStack;

    UPROPERTY()
    TObjectPtr<class UCameraComponent> CachedCamera;
};
