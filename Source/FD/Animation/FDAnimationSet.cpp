// Copyright YoungSterYSD. All Rights Reserved.

#include "Animation/FDAnimationSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDAnimationSet)

UAnimSequence* UFDAnimationSet::FindBestMatch(const FGameplayTagContainer& Query) const
{
    if (Query == CachedQuery)
    {
        return CachedResult.Get();
    }
    CachedQuery = Query;

    int32 BestScore = 0;
    UAnimSequence* BestAnim = nullptr;

    for (const FFDAnimSequenceGroup& Group : Groups)
    {
        int32 GroupScore = 0;
        for (const FGameplayTag& Tag : Group.GroupTags)
        {
            if (Query.HasTag(Tag))
            {
                ++GroupScore;
            }
        }

        const int32 GroupTagCount = Group.GroupTags.Num();

        for (const FFDAnimSequenceEntry& Entry : Group.Entries)
        {
            int32 Score = GroupScore;
            for (const FGameplayTag& Tag : Entry.MatchTags)
            {
                if (Query.HasTag(Tag))
                {
                    ++Score;
                }
            }

            if (Score > BestScore)
            {
                BestScore = Score;
                // Get() 只读指针，线程安全，不触发同步加载
                BestAnim = Entry.Animation.Get();
                if (!BestAnim)
                {
                    continue;
                }
                if (Score == GroupTagCount + Entry.MatchTags.Num())
                {
                    CachedResult = BestAnim;
                    return BestAnim;
                }
            }
        }
    }

    CachedResult = BestAnim;
    return BestAnim;
}

void UFDAnimationSet::PostLoad()
{
    Super::PostLoad();

    // Game Thread 预加载所有软引用，确保后续 Get() 不返回空
    for (FFDAnimSequenceGroup& Group : Groups)
    {
        for (FFDAnimSequenceEntry& Entry : Group.Entries)
        {
            Entry.Animation.LoadSynchronous();
        }
    }
}
