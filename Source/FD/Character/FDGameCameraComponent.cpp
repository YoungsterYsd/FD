// Copyright YoungSterYSD. All Rights Reserved.

#include "FDGameCameraComponent.h"
#include "Camera/FDCameraMode.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDGameCameraComponent)

UFDGameCameraComponent::UFDGameCameraComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UFDGameCameraComponent::ActivateDefaultCamera()
{
    if (DefaultModeClass)
    {
        PushCameraMode(DefaultModeClass);
    }
}

void UFDGameCameraComponent::PushCameraMode(TSubclassOf<UFDCameraMode> ModeClass)
{
    if (!ModeClass)
        return;

    UFDCameraMode* NewMode = NewObject<UFDCameraMode>(this, ModeClass);
    if (ActiveMode)
    {
        ModeStack.Push(ActiveMode);
    }
    ActiveMode = NewMode;
    UE_LOG(LogFD, Log, TEXT("Camera: Pushed %s"), *ModeClass->GetName());
}

void UFDGameCameraComponent::PopCameraMode()
{
    if (ModeStack.Num() > 0)
    {
        ActiveMode = ModeStack.Pop();
        UE_LOG(LogFD, Log, TEXT("Camera: Popped, restored previous mode"));
    }
}

void UFDGameCameraComponent::AdjustZoom(float Delta)
{
    if (ActiveMode)
    {
        ActiveMode->AdjustZoom(Delta);
    }
}

void UFDGameCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // B 阶段只维护模式栈，实际相机位置由 PlayerController 查询 UpdateView 计算
}
