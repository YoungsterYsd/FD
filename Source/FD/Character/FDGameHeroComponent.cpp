// Copyright YoungSterYSD. All Rights Reserved.

#include "FDGameHeroComponent.h"
#include "Input/FDInputConfig.h"
#include "Player/FDPlayerController.h"
#include "AbilitySystem/FDAbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayTags/FDGameplayTags.h"
#include "FDGameCameraComponent.h"
#include "LogChannels/FDLogChannels.h"
#include "Character/FDCharacter.h"
#include "Character/FDCharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDGameHeroComponent)

UFDGameHeroComponent::UFDGameHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UFDGameHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	UE_LOG(LogFD, Log, TEXT("HeroComponent::InitializePlayerInput called"));

	if (!InputConfig)
	{
		UE_LOG(LogFD, Warning, TEXT("HeroComponent: InputConfig is null, input binding skipped"));
		return;
	}

	UEnhancedInputComponent* EnhancedIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedIC)
	{
		UE_LOG(LogFD, Error, TEXT("HeroComponent: InputComponent is not UEnhancedInputComponent! Input binding failed"));
		return;
	}

	APawn* OwnerPawn = GetPawn<APawn>();
	AFDPlayerController* PC = OwnerPawn ? Cast<AFDPlayerController>(OwnerPawn->GetController()) : nullptr;

	UE_LOG(LogFD, Log, TEXT("HeroComponent: OwnerPawn=%s, PC=%s"), *GetNameSafe(OwnerPawn), *GetNameSafe(PC));

	// 绑定 NativeInputActions —— 直接回调
	for (const FFDTaggedInputAction& Binding : InputConfig->NativeInputActions)
	{
		if (!Binding.InputAction)
		{
			continue;
		}

		if (Binding.InputTag == FDGameplayTags::InputTag_Move)
		{
			EnhancedIC->BindAction(Binding.InputAction, ETriggerEvent::Triggered,
				PC, &AFDPlayerController::HandleWASDMove);
			UE_LOG(LogFD, Log, TEXT("HeroComponent: Bound InputTag.Move -> %s"), *GetNameSafe(Binding.InputAction.Get()));
		}
		else if (Binding.InputTag == FDGameplayTags::InputTag_Interact)
		{
			EnhancedIC->BindAction(Binding.InputAction, ETriggerEvent::Started,
				this, &UFDGameHeroComponent::Input_Interact);
		}
		else if (Binding.InputTag == FDGameplayTags::InputTag_Jump)
		{
			EnhancedIC->BindAction(Binding.InputAction, ETriggerEvent::Started,
				this, &UFDGameHeroComponent::Input_Jump);
			EnhancedIC->BindAction(Binding.InputAction, ETriggerEvent::Completed,
				this, &UFDGameHeroComponent::Input_StopJump);
			UE_LOG(LogFD, Log, TEXT("HeroComponent: Bound InputTag.Jump -> %s"), *GetNameSafe(Binding.InputAction.Get()));
		}
		else if (Binding.InputTag == FDGameplayTags::InputTag_Camera_Zoom)
		{
			// 直接绑定到 CameraComponent，不经过 PlayerController
			AActor* Owner = GetOwner();
			UFDGameCameraComponent* CamComp = Owner ? Owner->FindComponentByClass<UFDGameCameraComponent>() : nullptr;
			if (CamComp)
			{
				EnhancedIC->BindAction(Binding.InputAction, ETriggerEvent::Triggered,
					CamComp, &UFDGameCameraComponent::AdjustZoom);
			}
			UE_LOG(LogFD, Log, TEXT("HeroComponent: Bound InputTag.Camera.Zoom -> %s"), *GetNameSafe(Binding.InputAction.Get()));
		}
		else if (Binding.InputTag == FDGameplayTags::InputTag_ClickMove)
		{
			EnhancedIC->BindAction(Binding.InputAction, ETriggerEvent::Started,
				PC, &AFDPlayerController::HandleClickToMove);
			UE_LOG(LogFD, Log, TEXT("HeroComponent: Bound InputTag.ClickMove -> %s"), *GetNameSafe(Binding.InputAction.Get()));
		}
	}

	// 绑定 AbilityInputActions —— 按下/释放 传递 InputTag
	for (const FFDTaggedInputAction& Binding : InputConfig->AbilityInputActions)
	{
		if (!Binding.InputAction)
		{
			continue;
		}

		const FGameplayTag Tag = Binding.InputTag;

		EnhancedIC->BindAction(Binding.InputAction, ETriggerEvent::Started,
			this, &UFDGameHeroComponent::Input_AbilityPressed, Tag);
		EnhancedIC->BindAction(Binding.InputAction, ETriggerEvent::Completed,
			this, &UFDGameHeroComponent::Input_AbilityReleased, Tag);
	}
}

void UFDGameHeroComponent::Input_Move(const FInputActionValue& Value)
{
	// 实际由 PlayerController 处理（通过 NativeInputActions 绑定到 PC::HandleWASDMove）
}

void UFDGameHeroComponent::Input_AbilityPressed(FGameplayTag InputTag)
{
	UE_LOG(LogFD, Log, TEXT("Ability Pressed: %s"), *InputTag.ToString());

	// Route to ASC for ability activation
	if (APawn* Pawn = GetPawn<APawn>())
	{
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Pawn))
		{
			if (UFDAbilitySystemComponent* ASC = Cast<UFDAbilitySystemComponent>(ASI->GetAbilitySystemComponent()))
			{
				ASC->AbilityInputTagPressed(InputTag);
				return;
			}
		}
		// Fallback: ASC on PlayerState
		if (APlayerState* PS = Pawn->GetPlayerState())
		{
			if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(PS))
			{
				if (UFDAbilitySystemComponent* ASC = Cast<UFDAbilitySystemComponent>(ASI->GetAbilitySystemComponent()))
				{
					ASC->AbilityInputTagPressed(InputTag);
				}
			}
		}
	}
}

void UFDGameHeroComponent::Input_AbilityReleased(FGameplayTag InputTag)
{
	UE_LOG(LogFD, Log, TEXT("Ability Released: %s"), *InputTag.ToString());

	// Route to ASC
	if (APawn* Pawn = GetPawn<APawn>())
	{
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Pawn))
		{
			if (UFDAbilitySystemComponent* ASC = Cast<UFDAbilitySystemComponent>(ASI->GetAbilitySystemComponent()))
			{
				ASC->AbilityInputTagReleased(InputTag);
				return;
			}
		}
		if (APlayerState* PS = Pawn->GetPlayerState())
		{
			if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(PS))
			{
				if (UFDAbilitySystemComponent* ASC = Cast<UFDAbilitySystemComponent>(ASI->GetAbilitySystemComponent()))
				{
					ASC->AbilityInputTagReleased(InputTag);
				}
			}
		}
	}
}

void UFDGameHeroComponent::Input_Interact(const FInputActionValue& Value)
{
	UE_LOG(LogFD, Log, TEXT("Interact pressed"));
}

void UFDGameHeroComponent::Input_Jump(const FInputActionValue& Value)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	// 检查 MovementLocked
	if (const APlayerState* PS = Pawn->GetPlayerState())
	{
		if (const UAbilitySystemComponent* ASC = PS->FindComponentByClass<UAbilitySystemComponent>())
		{
			if (ASC->HasMatchingGameplayTag(FDGameplayTags::Status_MovementLocked))
			{
				return;
			}
		}
	}

	if (ACharacter* Character = Cast<ACharacter>(Pawn))
	{
		Character->Jump();
		UE_LOG(LogFD, Log, TEXT("Jump pressed"));
	}
}

void UFDGameHeroComponent::Input_StopJump(const FInputActionValue& Value)
{
	if (ACharacter* Character = Cast<ACharacter>(GetPawn<APawn>()))
	{
		Character->StopJumping();
	}
}

// ===== Battle State =====

void UFDGameHeroComponent::SetInBattle(bool bEnabled)
{
	if (bInBattle == bEnabled) { return; }
	bInBattle = bEnabled;

	if (!bEnabled)
	{
		if (AFDCharacter* Char = Cast<AFDCharacter>(GetPawn<APawn>()))
		{
			Char->GetFDCharacterMovement()->SetBattleFacingTarget(nullptr);
		}
	}
}

void UFDGameHeroComponent::SetBattleTarget(AActor* Target)
{
	BattleTarget = Target;

	if (AFDCharacter* Char = Cast<AFDCharacter>(GetPawn<APawn>()))
	{
		Char->GetFDCharacterMovement()->SetBattleFacingTarget(Target);
	}
}
