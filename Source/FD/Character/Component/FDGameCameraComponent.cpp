// Copyright YoungSterYSD. All Rights Reserved.

#include "FDGameCameraComponent.h"
#include "Camera/FDCameraMode.h"
#include "LogChannels/FDLogChannels.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDGameCameraComponent)

UFDGameCameraComponent::UFDGameCameraComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UFDGameCameraComponent::BeginPlay()
{
    Super::BeginPlay();

    if (AActor* Owner = GetOwner())
    {
        CachedCamera = Owner->FindComponentByClass<UCameraComponent>();
        if (!CachedCamera)
        {
            UE_LOG(LogFD, Warning, TEXT("FDGameCameraComponent: No UCameraComponent found on %s"), *Owner->GetName());
        }
    }
}

void UFDGameCameraComponent::ActivateDefaultCamera()
{
    if (DefaultModeClass)
    {
        PushCameraMode(DefaultModeClass);
    }
    else
    {
        UE_LOG(LogFD, Warning, TEXT("Camera: DefaultModeClass is null, camera not activated"));
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

void UFDGameCameraComponent::AdjustZoom(const FInputActionValue& Value)
{
    if (ActiveMode)
    {
        ActiveMode->AdjustZoom(Value.Get<float>());
    }
}

void UFDGameCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    static bool bLoggedOnce = false;
    if (!bLoggedOnce)
    {
        UE_LOG(LogFD, Log, TEXT("Camera: TickComponent running, ActiveMode=%s, CachedCamera=%s"),
            *GetNameSafe(ActiveMode), *GetNameSafe(CachedCamera));
        bLoggedOnce = true;
    }

    if (!ActiveMode || !CachedCamera)
    {
        return;
    }

    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    FVector OutLocation;
    FRotator OutRotation;
    float OutFOV;
    ActiveMode->UpdateView(DeltaTime, Owner, OutLocation, OutRotation, OutFOV);

    CachedCamera->SetWorldLocation(OutLocation);
    CachedCamera->SetWorldRotation(OutRotation);
    CachedCamera->SetFieldOfView(OutFOV);
}
