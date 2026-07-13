// Copyright YoungSterYSD. All Rights Reserved.

#include "FDPlayerController.h"
#include "FDPlayerState.h"
#include "Character/FDGameCameraComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTags/FDGameplayTags.h"
#include "Camera/FDCameraMode.h"
#include "AbilitySystem/FDAbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "LogChannels/FDLogChannels.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Engine/LocalPlayer.h"
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

    UpdateControlRotationFromCamera();

    // Process ability input queue (calls TryActivateAbility for pressed InputTags)
    if (APawn* ControlledPawn = GetPawn())
    {
        if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(ControlledPawn))
        {
            if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
            {
                if (UFDAbilitySystemComponent* FD_ASC = Cast<UFDAbilitySystemComponent>(ASC))
                {
                    FD_ASC->ProcessAbilityInput(DeltaSeconds, false);
                }
            }
        }
        // Fallback: ASC might be on PlayerState
        if (APlayerState* PS = ControlledPawn->GetPlayerState())
        {
            if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(PS))
            {
                if (UFDAbilitySystemComponent* FD_ASC = Cast<UFDAbilitySystemComponent>(ASI->GetAbilitySystemComponent()))
                {
                    FD_ASC->ProcessAbilityInput(DeltaSeconds, false);
                }
            }
        }
    }
}

void AFDPlayerController::UpdateControlRotationFromCamera()
{
    if (APawn* ControlledPawn = GetPawn())
    {
        if (UCameraComponent* Cam = ControlledPawn->FindComponentByClass<UCameraComponent>())
        {
            SetControlRotation(Cam->GetComponentRotation());
        }
    }
}

void AFDPlayerController::HandleWASDMove(const FInputActionValue& Value)
{
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn)
    {
        return;
    }

    // 检查 PlayerState 的 ASC 是否有 Status.MovementLocked Tag（ASC 在 PlayerState 上！）
    if (const APlayerState* PS = ControlledPawn->GetPlayerState())
    {
        if (const UAbilitySystemComponent* ASC = PS->FindComponentByClass<UAbilitySystemComponent>())
        {
            if (ASC->HasMatchingGameplayTag(FDGameplayTags::Status_MovementLocked))
            {
                return;
            }
        }
    }

    // WASD 按下时取消点击寻路
    bClickMoveActive = false;

    const FVector2D MovementVector = Value.Get<FVector2D>();
    const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    ControlledPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
    ControlledPawn->AddMovementInput(RightDirection, MovementVector.X);
}

void AFDPlayerController::HandleClickToMove(const FInputActionValue& Value)
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

UFDGameCameraComponent* AFDPlayerController::GetCameraComponent() const
{
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn)
    {
        return nullptr;
    }

    return ControlledPawn->FindComponentByClass<UFDGameCameraComponent>();
}
