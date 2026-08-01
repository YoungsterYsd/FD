// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "FDPlayerController.generated.h"

class UFDGameCameraComponent;
class UInputMappingContext;
class UFDAbilitySystemComponent;

/**
 * FD 项目 PlayerController —— 处理 WASD 移动、相机更新与 GAS 输入。
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

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void SetupInputComponent() override;

private:
    /** 缓存的 ASC 引用，在 OnPossess 中设置，Tick 中直接使用（避免逐帧查找） */
    UPROPERTY()
    TObjectPtr<UFDAbilitySystemComponent> CachedASC;

    /** 获取 Pawn 上的 CameraComponent */
    UFDGameCameraComponent* GetCameraComponent() const;

    /** 输入映射上下文 */
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputMappingContext> InputMappingContext;
};
