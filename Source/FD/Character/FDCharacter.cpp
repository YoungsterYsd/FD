// Copyright YoungSterYSD. All Rights Reserved.

#include "FDCharacter.h"
#include "FDGamePawnExtensionComponent.h"
#include "FDGameCameraComponent.h"
#include "FDGameHeroComponent.h"
#include "AbilitySystem/Attributes/FDAttributeComponent.h"
#include "LogChannels/FDLogChannels.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Camera/CameraComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDCharacter)

AFDCharacter::AFDCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PawnExtComp  = CreateDefaultSubobject<UFDGamePawnExtensionComponent>(TEXT("PawnExtension"));
    CameraComp   = CreateDefaultSubobject<UFDGameCameraComponent>(TEXT("GameCamera"));
    HeroComp     = CreateDefaultSubobject<UFDGameHeroComponent>(TEXT("Hero"));
    AttributeComp = CreateDefaultSubobject<UFDAttributeComponent>(TEXT("Attribute"));

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(RootComponent);
    Camera->bUsePawnControlRotation = false;
    Camera->FieldOfView = 90.f;

    // 俯视角移动配置
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    MoveComp->bConstrainToPlane = true;
    MoveComp->bSnapToPlaneAtStart = true;
    MoveComp->bOrientRotationToMovement = true;
    MoveComp->RotationRate = FRotator(0.f, 640.f, 0.f);
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
}

void AFDCharacter::PreInitializeComponents()
{
    Super::PreInitializeComponents();
    UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AFDCharacter::BeginPlay()
{
    UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(
        this, UGameFrameworkComponentManager::NAME_GameActorReady);
    Super::BeginPlay();
}

void AFDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    if (HeroComp)
    {
        HeroComp->InitializePlayerInput(PlayerInputComponent);
    }
}

void AFDCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
    Super::EndPlay(EndPlayReason);
}
