// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "FDPlayerController.generated.h"

class UFDGameCameraComponent;

/**
 * FD 项目 PlayerController —— 处理移动、寻路、相机更新。
 */
UCLASS()
class FD_API AFDPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AFDPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void Tick(float DeltaSeconds) override;

    /** WASD 键盘移动 */
    void HandleWASDMove(const struct FInputActionValue& Value);

    /** 鼠标点击寻路 */
    void HandleClickToMove();

    /** 滚轮缩放 */
    void HandleMouseWheelZoom(const struct FInputActionValue& Value);

protected:
    virtual void BeginPlay() override;

private:
    /** 执行寻路到目标点 */
    void SetNewMoveDestination(const FVector& Destination);

    /** 获取 Pawn 上的 CameraComponent */
    UFDGameCameraComponent* GetCameraComponent() const;

    bool bClickMoveActive = false;
    FVector CachedDestination = FVector::ZeroVector;
};
