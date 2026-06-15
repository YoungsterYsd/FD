// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_EndAbility.generated.h"

/**
 * AnimNotify that ends the currently active UFDGameplayAbility.
 *
 * Use this at the end of an attack montage to trigger GA cleanup.
 * Replaces the previous pattern of calling EndAbility() at the end of ActivateAbility().
 */
UCLASS(Blueprintable, EditInlineNew, meta = (DisplayName = "End Ability"))
class FD_API UAN_EndAbility : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};
