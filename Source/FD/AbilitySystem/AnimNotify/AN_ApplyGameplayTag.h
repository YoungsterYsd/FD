// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AN_ApplyGameplayTag.generated.h"

/**
 * AnimNotify that adds or removes a GameplayTag on the owning character's ASC.
 */
UCLASS(Blueprintable, EditInlineNew, meta = (DisplayName = "Apply Gameplay Tag"))
class FD_API UAN_ApplyGameplayTag : public UAnimNotify
{
	GENERATED_BODY()

public:
	/** The GameplayTag to add or remove. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tag")
	FGameplayTag Tag;

	/** If true, add the tag; if false, remove it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tag")
	bool bAdd = true;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};
