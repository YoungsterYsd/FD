// Copyright YoungSterYSD. All Rights Reserved.

#include "FDCharacter.h"
#include "FDGamePawnExtensionComponent.h"
#include "FDGameCameraComponent.h"
#include "FDGameHeroComponent.h"
#include "LogChannels/FDLogChannels.h"
#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDCharacter)

AFDCharacter::AFDCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PawnExtComp = CreateDefaultSubobject<UFDGamePawnExtensionComponent>(TEXT("PawnExtension"));
    CameraComp  = CreateDefaultSubobject<UFDGameCameraComponent>(TEXT("Camera"));
    HeroComp    = CreateDefaultSubobject<UFDGameHeroComponent>(TEXT("Hero"));
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
