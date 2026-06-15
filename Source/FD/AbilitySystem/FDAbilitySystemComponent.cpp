// Copyright YoungSterYSD. All Rights Reserved.

#include "FDAbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "LogChannels/FDLogChannels.h"
#include "AbilitySystem/AnimNotify/ANS_InputBuffer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDAbilitySystemComponent)

void UFDAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	UE_LOG(LogFDGAS, Verbose, TEXT("ASC::AbilityInputTagPressed - Tag=%s"), *InputTag.ToString());

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTag(InputTag))
		{
			continue;
		}

		InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
		InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
	}

	// Forward input to any active InputBuffer ANS for combo routing
	if (AActor* Owner = GetOwnerActor())
	{
		if (UANS_InputBuffer** Buffer = UANS_InputBuffer::ActiveBuffers.Find(Owner))
		{
			(*Buffer)->BufferInput(InputTag);
		}
	}
}

void UFDAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTag(InputTag))
		{
			continue;
		}

		InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
		InputHeldSpecHandles.Remove(AbilitySpec.Handle);
	}
}

void UFDAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	// Process pressed abilities: try to activate each
	for (const FGameplayAbilitySpecHandle& Handle : InputPressedSpecHandles)
	{
		if (!Handle.IsValid()) continue;

		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (!Spec || !Spec->Ability) continue;

		if (Spec->IsActive())
		{
			AbilitySpecInputPressed(*Spec);
		}
		AbilitiesToActivate.AddUnique(Handle);
	}

	// Activate abilities outside the iteration to avoid modifying while iterating
	for (const FGameplayAbilitySpecHandle& Handle : AbilitiesToActivate)
	{
		TryActivateAbility(Handle);
	}

	// Process held abilities: notify input held
	for (const FGameplayAbilitySpecHandle& Handle : InputHeldSpecHandles)
	{
		if (!Handle.IsValid())
		{
			continue;
		}

		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (Spec && Spec->IsActive())
		{
			// Input is still held this frame
		}
	}

	// Process released abilities: notify input released
	for (const FGameplayAbilitySpecHandle& Handle : InputReleasedSpecHandles)
	{
		if (!Handle.IsValid())
		{
			continue;
		}

		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if (Spec && Spec->IsActive())
		{
			AbilitySpecInputReleased(*Spec);
		}
	}

	// Clear per-frame queues
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}
