// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "ANS_InputBuffer.generated.h"

class UAbilitySystemComponent;
class UANS_InputBuffer;

/**
 * AnimNotifyState that buffers input during a montage window and routes them to combo abilities
 * when the window ends.
 *
 * Works together with UFDAbilitySystemComponent::AbilityInputTagPressed() which forwards
 * input tags to active InputBuffer instances.
 */
UCLASS(Blueprintable, EditInlineNew, meta = (DisplayName = "Input Buffer"))
class FD_API UANS_InputBuffer : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	/** Maps an input tag (e.g. InputTag.Ability.1) to the combo ability tag to activate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InputBuffer")
	TMap<FGameplayTag, FGameplayTag> InputTagToAbilityTag;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	/**
	 * Called by UFDAbilitySystemComponent when input is pressed to buffer it.
	 * @param InputTag - The pressed input GameplayTag.
	 */
	void BufferInput(const FGameplayTag& InputTag);

	/** Registry of all currently active InputBuffer instances, keyed by owning Actor. */
	static TMap<TWeakObjectPtr<AActor>, UANS_InputBuffer*> ActiveBuffers;

private:
	/** Input tags buffered during this window. */
	TArray<FGameplayTag> BufferedInputs;

	/** Cached ASC pointer for efficiency. */
	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;
};
