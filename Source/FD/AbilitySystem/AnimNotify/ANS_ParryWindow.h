// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "ANS_ParryWindow.generated.h"

/**
 * AnimNotifyState that adds a parry tag to the owning character's ASC for the duration
 * of the notify window.
 *
 * A parry window allows the character to deflect incoming attacks during this timeframe.
 * The actual parry logic (e.g. deflection check on hit) is implemented in the damage pipeline.
 */
UCLASS(Blueprintable, EditInlineNew, meta = (DisplayName = "Parry Window"))
class FD_API UANS_ParryWindow : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	/** Parry window duration in seconds (0 = entire NotifyState span). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parry")
	float ParryWindowDuration = 0.0f;

	/** The GameplayTag added during the parry window (e.g. Status.Parrying). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parry")
	FGameplayTag ParryTag;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};
