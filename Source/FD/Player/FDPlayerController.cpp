// Copyright YoungSterYSD. All Rights Reserved.

#include "FDPlayerController.h"
#include "FDPlayerState.h"
#include "Character/Component/FDGameCameraComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "Camera/FDCameraMode.h"
#include "AbilitySystem/FDAbilitySystemComponent.h"
#include "LogChannels/FDLogChannels.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Engine/LocalPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDPlayerController)

AFDPlayerController::AFDPlayerController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    UE_LOG(LogFD, Log, TEXT("AFDPlayerController created"));
}

void AFDPlayerController::BeginPlay()
{
    Super::BeginPlay();
}

void AFDPlayerController::OnPossess(APawn* InPawn)
{
    UE_LOG(LogFD, Log, TEXT("AFDPlayerController::OnPossess called, Pawn=%s"), *GetNameSafe(InPawn));

    Super::OnPossess(InPawn);

    // 初始化 GAS ActorInfo：告诉 ASC AvatarActor 是当前控制的 Character
    if (AFDPlayerState* PS = InPawn->GetPlayerState<AFDPlayerState>())
    {
        if (UFDAbilitySystemComponent* ASC = PS->GetFDAbilitySystemComponent())
        {
            ASC->InitAbilityActorInfo(PS, InPawn);
            UE_LOG(LogFD, Log, TEXT("InitAbilityActorInfo - Owner=%s, Avatar=%s"),
                *PS->GetName(), *InPawn->GetName());
        }
    }

    // Possess 后 Pawn 已就位，此时激活默认相机模式
    if (UFDGameCameraComponent* CamComp = GetCameraComponent())
    {
        CamComp->ActivateDefaultCamera();
    }

    // 缓存 ASC 引用，供 Tick 直接使用（避免逐帧 GetPawn + Cast 链）
    if (AFDPlayerState* PS = InPawn->GetPlayerState<AFDPlayerState>())
    {
        CachedASC = PS->GetFDAbilitySystemComponent();
    }

    // 注入 InputMappingContext
    if (IsLocalPlayerController() && InputMappingContext)
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(InputMappingContext, 0);
        }
    }
}

void AFDPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
}

void AFDPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // 使用缓存的 ASC 引用处理技能输入（OnPossess 中缓存，避免逐帧查找）
    if (CachedASC)
    {
        CachedASC->ProcessAbilityInput(DeltaSeconds, false);
    }
}

void AFDPlayerController::HandleWASDMove(const FInputActionValue& Value)
{
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

UFDGameCameraComponent* AFDPlayerController::GetCameraComponent() const
{
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn)
    {
        return nullptr;
    }

    return ControlledPawn->FindComponentByClass<UFDGameCameraComponent>();
}
