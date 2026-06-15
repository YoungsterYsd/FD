// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PlaySound.generated.h"

class USoundBase;

/**
 * AnimNotify that plays a sound at the owning character's location.
 */
UCLASS(Blueprintable, EditInlineNew, meta = (DisplayName = "Play Sound"))
class FD_API UAN_PlaySound : public UAnimNotify
{
	GENERATED_BODY()

public:
	/** Sound asset to play. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> Sound;

	/** Volume multiplier (1.0 = full volume). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float VolumeMultiplier = 1.0f;

	/** Pitch multiplier (1.0 = normal pitch). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float PitchMultiplier = 1.0f;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};
