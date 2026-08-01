// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/Data/FDAbilitySet.h"
#include "FDSkillComponent.generated.h"

/**
 * Skill component — manages GA grant/revoke lifecycle.
 *
 * Mounted on AFDCharacter. At BeginPlay, grants the DefaultAbilitySet
 * to the owner's ASC. Supports runtime grant/revoke of additional
 * AbilitySets with handle tracking for proper cleanup.
 */
UCLASS(BlueprintType)
class FD_API UFDSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFDSkillComponent();

	/** Default ability set granted at BeginPlay. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FD|Skill")
	TObjectPtr<UFDAbilitySet> DefaultAbilitySet;

	/**
	 * Grant an AbilitySet to the owner's ASC.
	 * @param Set - The AbilitySet to grant.
	 */
	void GrantAbilitySet(UFDAbilitySet* Set);

	/**
	 * Revoke a previously granted AbilitySet.
	 * @param Set - The AbilitySet to revoke.
	 */
	void RevokeAbilitySet(UFDAbilitySet* Set);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** Tracked handles for each granted AbilitySet. */
	UPROPERTY()
	TMap<TObjectPtr<UFDAbilitySet>, FFDAbilitySet_GrantedHandles> GrantedHandlesMap;
};
