// Copyright YoungSterYSD. All Rights Reserved.

#include "FDExperienceManagerComponent.h"
#include "FDExperienceDefinition.h"
#include "../GameplayTags/FDGameplayTags.h"
#include "../LogChannels/FDLogChannels.h"
#include "Engine/AssetManager.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeatureAction.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/GameStateBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDExperienceManagerComponent)

UFDExperienceManagerComponent::UFDExperienceManagerComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UFDExperienceManagerComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UFDExperienceManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

void UFDExperienceManagerComponent::SetCurrentExperience(const UFDExperienceDefinition* InExperience)
{
    if (!IsValid(InExperience))
    {
        UE_LOG(LogFD, Error, TEXT("SetCurrentExperience: Invalid Experience"));
        return;
    }

    if (LoadState != EFDExperienceLoadState::Unloaded)
    {
        UE_LOG(LogFD, Warning, TEXT("SetCurrentExperience: Already loading/loaded, ignoring"));
        return;
    }

    CurrentExperience = InExperience;
    LoadState = EFDExperienceLoadState::Loading;
    UE_LOG(LogFD, Log, TEXT("Experience loading started: %s"), *GetNameSafe(InExperience));

    StartExperienceLoad();
}

void UFDExperienceManagerComponent::StartExperienceLoad()
{
    // 通过 AssetManager 异步加载 Experience 资产包
    if (UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
    {
        TArray<FSoftObjectPath> AssetsToLoad;
        const FSoftObjectPath ExperiencePath(CurrentExperience);
        AssetsToLoad.Add(ExperiencePath);

        FStreamableDelegate OnLoadedDelegate = FStreamableDelegate::CreateUObject(
            this, &UFDExperienceManagerComponent::OnAssetLoaded);

        AssetManager->GetStreamableManager().RequestAsyncLoad(
            AssetsToLoad, OnLoadedDelegate);
    }
    else
    {
        // 资产管理器未就绪，直接进入下一阶段
        OnAssetLoaded();
    }
}

void UFDExperienceManagerComponent::OnAssetLoaded()
{
    LoadState = EFDExperienceLoadState::ExecutingActions;
    UE_LOG(LogFD, Log, TEXT("Experience assets loaded, executing actions..."));

    ExecuteGameFeatureActions();
}

void UFDExperienceManagerComponent::ExecuteGameFeatureActions()
{
    const TArray<TObjectPtr<UGameFeatureAction>>& Actions = CurrentExperience->Actions;

    if (Actions.Num() == 0)
    {
        // 没有 Action，直接完成加载
        FinishExperienceLoad();
        return;
    }

    // 遍历执行每个 Action
    // GameFeatureAction 的完整实现在后续阶段补完，此处预留循环结构
    FGameFeatureActivatingContext Context;

    for (const TObjectPtr<UGameFeatureAction>& Action : Actions)
    {
        if (IsValid(Action))
        {
            Action->OnGameFeatureActivating(Context);
        }
    }

    FinishExperienceLoad();
}

void UFDExperienceManagerComponent::FinishExperienceLoad()
{
    LoadState = EFDExperienceLoadState::Loaded;
    UE_LOG(LogFD, Log, TEXT("Experience fully loaded"));

    // 调用同步委托 —— 后续阶段可增加 GameplayMessageRouter 广播
    OnExperienceLoaded.Broadcast(CurrentExperience);
}
