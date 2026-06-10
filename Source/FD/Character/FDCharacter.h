// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "FDCharacter.generated.h"

class UFDGamePawnExtensionComponent;
class UFDGameCameraComponent;
class UFDGameHeroComponent;

/**
 * FD 项目角色基类。
 * 挂载 PawnExtensionComponent + CameraComponent，注册到 GameFrameworkComponentManager。
 */
UCLASS()
class FD_API AFDCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AFDCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UFDGamePawnExtensionComponent* GetPawnExtensionComponent() const { return PawnExtComp; }
    UFDGameCameraComponent* GetCameraComponent() const { return CameraComp; }
    UFDGameHeroComponent* GetHeroComponent() const { return HeroComp; }

protected:
    virtual void PreInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UFDGamePawnExtensionComponent> PawnExtComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UFDGameCameraComponent> CameraComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UFDGameHeroComponent> HeroComp;
};
