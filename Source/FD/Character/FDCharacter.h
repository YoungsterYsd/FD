// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"
#include "FDCharacter.generated.h"

class UFDGamePawnExtensionComponent;
class UFDGameCameraComponent;
class UFDGameHeroComponent;
class UCameraComponent;
class UFDAttributeComponent;
class UFDSkillComponent;

/**
 * FD Character base class.
 * Mounts PawnExtensionComponent + CameraComponent + HeroComponent +
 * SkillComponent + AttributeComponent + Camera,
 * registers with GameFrameworkComponentManager.
 */
UCLASS()
class FD_API AFDCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AFDCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // ===== Component Accessors =====
    UFDGamePawnExtensionComponent* GetPawnExtensionComponent() const { return PawnExtComp; }
    UFDGameCameraComponent* GetCameraComponent() const { return CameraComp; }
    UFDGameHeroComponent* GetHeroComponent() const { return HeroComp; }
    UCameraComponent* GetCamera() const { return Camera; }
    UFDAttributeComponent* GetAttributeComponent() const { return AttributeComp; }
    UFDSkillComponent* GetSkillComponent() const { return SkillComp; }

protected:
    virtual void PreInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void PostInitializeComponents() override;

    // ===== Components =====
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UFDGamePawnExtensionComponent> PawnExtComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UFDGameCameraComponent> CameraComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UFDGameHeroComponent> HeroComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UFDSkillComponent> SkillComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UFDAttributeComponent> AttributeComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UCameraComponent> Camera;
};
