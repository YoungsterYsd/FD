// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"
#include "FDFeatureAction_WorldActionBase.generated.h"

class FDelegateHandle;
class UGameInstance;
struct FWorldContext;

/**
 * GameFeatureAction 基类 —— 延迟到 World 就绪后才执行注入。
 * 子类只需实现 AddToWorld()，不必关心时序问题。
 */
UCLASS(Abstract)
class FD_API UFDFeatureAction_WorldActionBase : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

private:
	void HandleGameInstanceStart(UGameInstance* GameInstance, FGameFeatureStateChangeContext ChangeContext);

	/** 子类实现：World 就绪后执行注入逻辑 */
	virtual void AddToWorld(const FWorldContext& WorldContext,
	                        const FGameFeatureStateChangeContext& ChangeContext) PURE_VIRTUAL(UFDFeatureAction_WorldActionBase::AddToWorld,);

	TMap<FGameFeatureStateChangeContext, FDelegateHandle> GameInstanceStartHandles;
};
