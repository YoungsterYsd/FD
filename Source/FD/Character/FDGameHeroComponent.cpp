// Copyright YoungSterYSD. All Rights Reserved.

#include "FDGameHeroComponent.h"
#include "Input/FDInputConfig.h"
#include "Player/FDPlayerController.h"
#include "GameplayTags/FDGameplayTags.h"
#include "LogChannels/FDLogChannels.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDGameHeroComponent)

UFDGameHeroComponent::UFDGameHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UFDGameHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	if (!InputConfig)
	{
		return;
	}

	UEnhancedInputComponent* EnhancedIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedIC)
	{
		UE_LOG(LogFD, Error, TEXT("HeroComponent: InputComponent is not UEnhancedInputComponent"));
		return;
	}

	APawn* OwnerPawn = GetPawn<APawn>();
	AFDPlayerController* PC = OwnerPawn ? Cast<AFDPlayerController>(OwnerPawn->GetController()) : nullptr;

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
		}
		else if (Binding.InputTag == FDGameplayTags::InputTag_Interact)
		{
			EnhancedIC->BindAction(Binding.InputAction, ETriggerEvent::Started,
				this, &UFDGameHeroComponent::Input_Interact);
		}
		else if (Binding.InputTag == FDGameplayTags::InputTag_Jump)
		{
			// 预留
		}
		// 后续阶段的 InputTag.ClickMove / InputTag.Camera.Zoom 等在此扩展
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
	// 后续阶段: 经输入缓冲 → ASC::AbilityInputTagPressed(InputTag)
}

void UFDGameHeroComponent::Input_AbilityReleased(FGameplayTag InputTag)
{
	UE_LOG(LogFD, Log, TEXT("Ability Released: %s"), *InputTag.ToString());
	// 后续阶段: ASC::AbilityInputTagReleased(InputTag)
}

void UFDGameHeroComponent::Input_Interact(const FInputActionValue& Value)
{
	UE_LOG(LogFD, Log, TEXT("Interact pressed"));
}
