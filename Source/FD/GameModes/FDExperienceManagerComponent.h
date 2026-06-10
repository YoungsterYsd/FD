// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "Components/GameStateComponent.h"
#include "FDExperienceManagerComponent.generated.h"

class UFDExperienceDefinition;

/**
 * Experience 加载状态机。
 *
 * Unloaded → Loading → ExecutingActions → Loaded → Deactivating
 *
 * 加载完成后通过 GameplayMessageRouter 广播消息，
 * 同时触发同步委托供 C++ 和蓝图监听。
 */
enum class EFDExperienceLoadState : uint8
{
    Unloaded,
    Loading,
    ExecutingActions,
    Loaded,
    Deactivating
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnFDExperienceLoaded, const UFDExperienceDefinition*);

UCLASS()
class FD_API UFDExperienceManagerComponent : public UGameStateComponent
{
    GENERATED_BODY()

public:
    UFDExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    /**
     * 设置当前 Experience 并开始异步加载流程。
     * @param InExperience - 要加载的 Experience 数据资产
     */
    void SetCurrentExperience(const UFDExperienceDefinition* InExperience);

    /** 查询 Experience 是否已加载完毕 */
    bool IsExperienceLoaded() const { return LoadState == EFDExperienceLoadState::Loaded; }

    /** 获取当前 Experience */
    const UFDExperienceDefinition* GetCurrentExperience() const { return CurrentExperience; }

    /** 获取当前加载状态 */
    EFDExperienceLoadState GetLoadState() const { return LoadState; }

    /** 加载完成委托（同步回调，C++ 绑定） */
    FOnFDExperienceLoaded OnExperienceLoaded;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    void StartExperienceLoad();
    void OnAssetLoaded();
    void ExecuteGameFeatureActions();
    void FinishExperienceLoad();

    /** 当前加载的 Experience */
    UPROPERTY()
    TObjectPtr<const UFDExperienceDefinition> CurrentExperience;

    /** 当前加载状态 */
    EFDExperienceLoadState LoadState = EFDExperienceLoadState::Unloaded;



    /** 尚未完成的 Action 数量 */
    int32 NumPendingActions = 0;
};
