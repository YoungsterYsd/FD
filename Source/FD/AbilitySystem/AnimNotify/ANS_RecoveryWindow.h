// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "ANS_RecoveryWindow.generated.h"

/**
 * AnimNotifyState representing a recovery window where certain input actions can cancel
 * the current ability and transition to another action.
 *
 * During this window, a Status.Recovery tag is added. On each tick, input press handles
 * are checked against CancelInputTags; if any match, the current ability is cancelled.
 */
UCLASS(Blueprintable, EditInlineNew, meta = (DisplayName = "Recovery Window"))
class FD_API UANS_RecoveryWindow : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	/** Input tags that are allowed to cancel the current ability during this window. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	TArray<FGameplayTag> CancelInputTags;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};
