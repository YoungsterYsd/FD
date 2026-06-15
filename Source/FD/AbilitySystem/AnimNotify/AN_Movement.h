// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_Movement.generated.h"

/**
 * AnimNotify that moves the owning character.
 *
 * Supports instant teleport (bProcessMovement = false) or process-based movement
 * (bProcessMovement = true, reserved for future C5 AbilityTask integration).
 */
UCLASS(Blueprintable, EditInlineNew, meta = (DisplayName = "Movement"))
class FD_API UAN_Movement : public UAnimNotify
{
	GENERATED_BODY()

public:
	/** If true, interpolation-based movement; if false, instant teleport. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bProcessMovement = false;

	/** Movement distance in world units. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Distance = 200.0f;

	/** Direction offset relative to the character's forward vector (Forward/Right/Up). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector DirectionOffset = FVector::ForwardVector;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};
