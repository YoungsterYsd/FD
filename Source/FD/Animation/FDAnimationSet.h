// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimSequence.h"
#include "FDAnimationSet.generated.h"

/**
 * Single animation entry in the animation set.
 * Associates a gameplay tag combination with an animation sequence.
 */
USTRUCT(BlueprintType)
struct FD_API FFDAnimSequenceEntry
{
    GENERATED_BODY()

    /** Tag combination to match against (e.g. {CharState.Idle, CharState.InBattle, Locomotion.MoveDirection.F}) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FD")
    FGameplayTagContainer MatchTags;

    /** Animation to play when this entry is the best match */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FD")
    TSoftObjectPtr<UAnimSequence> Animation;
};

/**
 * A named group of animation entries for organisational purposes.
 * Use e.g. "Idle_InBattle", "Walk_NoCombat", "Run_InBattle" as group names.
 */
USTRUCT(BlueprintType)
struct FD_API FFDAnimSequenceGroup
{
    GENERATED_BODY()

    /** Display name shown in the editor (collapsible section) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FD")
    FName GroupName;

    /** Tags shared by ALL entries in this group. Scored as if each entry also carries them. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FD")
    FGameplayTagContainer GroupTags;

    /** Entries belonging to this group */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FD")
    TArray<FFDAnimSequenceEntry> Entries;
};

/**
 * Tag-driven animation lookup table.
 *
 * Organised into named groups for readability.
 * When queried, all entries across all groups compete.
 * Returns the entry with the highest number of matching tags.
 */
UCLASS(BlueprintType)
class FD_API UFDAnimationSet : public UDataAsset
{
    GENERATED_BODY()

public:
    /** Animation groups — flattened at query time so grouping is purely organisational. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FD")
    TArray<FFDAnimSequenceGroup> Groups;

    /**
     * Find the best-matching animation for a tag combination.
     * Searches across ALL groups; the group itself has no impact on the match.
     * @param Query - The current state tag container to match against.
     * @return The best matching animation, or nullptr if no entries match at all.
     */
    UFUNCTION(BlueprintCallable, Category = "FD|Animation")
    UAnimSequence* FindBestMatch(const FGameplayTagContainer& Query) const;

protected:
    virtual void PostLoad() override;

private:
    mutable FGameplayTagContainer CachedQuery;
    mutable TWeakObjectPtr<UAnimSequence> CachedResult;
};
