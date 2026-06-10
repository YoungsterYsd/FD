// Copyright YoungSterYSD. All Rights Reserved.

#include "FDPlayerController.h"
#include "Character/FDGameCameraComponent.h"
#include "Camera/FDCameraMode.h"
#include "LogChannels/FDLogChannels.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "InputActionValue.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDPlayerController)

AFDPlayerController::AFDPlayerController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void AFDPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // 激活默认相机模式
    if (UFDGameCameraComponent* CamComp = GetCameraComponent())
    {
        CamComp->ActivateDefaultCamera();
    }
}

void AFDPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // 驱动当前相机模式计算 View
    UFDGameCameraComponent* CamComp = GetCameraComponent();
    APawn* ControlledPawn = GetPawn();
    if (CamComp && ControlledPawn)
    {
        UFDCameraMode* ActiveMode = CamComp->GetActiveCameraMode();
        if (ActiveMode)
        {
            FVector OutLocation;
            FRotator OutRotation;
            float OutFOV;
            ActiveMode->UpdateView(DeltaSeconds, ControlledPawn, OutLocation, OutRotation, OutFOV);
            SetControlRotation(OutRotation);
        }
    }
}

void AFDPlayerController::HandleWASDMove(const FInputActionValue& Value)
{
    // WASD 按下时取消点击寻路
    bClickMoveActive = false;

    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn)
    {
        return;
    }

    const FVector2D MovementVector = Value.Get<FVector2D>();
    const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    ControlledPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
    ControlledPawn->AddMovementInput(RightDirection, MovementVector.X);
}

void AFDPlayerController::HandleClickToMove()
{
    bClickMoveActive = true;

    FHitResult Hit;
    if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        CachedDestination = Hit.Location;
        SetNewMoveDestination(CachedDestination);
    }
}

void AFDPlayerController::SetNewMoveDestination(const FVector& Destination)
{
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn)
    {
        return;
    }

    UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, Destination);
}

void AFDPlayerController::HandleMouseWheelZoom(const FInputActionValue& Value)
{
    if (UFDGameCameraComponent* CamComp = GetCameraComponent())
    {
        CamComp->AdjustZoom(Value.Get<float>());
    }
}

UFDGameCameraComponent* AFDPlayerController::GetCameraComponent() const
{
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn)
    {
        return nullptr;
    }

    return ControlledPawn->FindComponentByClass<UFDGameCameraComponent>();
}
