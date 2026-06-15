// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "ANS_IFrameWindow.generated.h"

/**
 * AnimNotifyState that adds an invincibility (i-frame) tag to the owning character's ASC
 * for the duration of the notify window.
 *
 * The damage execution should check for this tag and skip damage if present.
 * Default tag: Status.Dodging.
 */
UCLASS(Blueprintable, EditInlineNew, meta = (DisplayName = "IFrame Window"))
class FD_API UANS_IFrameWindow : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	/** The GameplayTag that grants invincibility (default: Status.Dodging). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IFrame")
	FGameplayTag IFrameTag;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};
