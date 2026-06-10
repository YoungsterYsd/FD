// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "GameplayTags/FDGameplayTags.h"
#include "FDGamePawnExtensionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnFDGameInitStateChanged,
	AActor*, OwningActor,
	FGameplayTag, NewState);

/**
 * Pawn 初始化状态协调者。
 * 挂在 Pawn 上，统一管理所有组件的 InitState 推进节奏。
 */
UCLASS()
class FD_API UFDGamePawnExtensionComponent : public UPawnComponent,
                                              public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UFDGamePawnExtensionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 查询当前是否已到达指定状态 */
	virtual bool HasReachedInitState(FGameplayTag DesiredState) const override;

	/** 获取当前状态 Tag */
	virtual FGameplayTag GetInitState() const override { return CurrentInitState; }

	/** 状态变更通知 */
	UPROPERTY(BlueprintAssignable)
	FOnFDGameInitStateChanged OnInitStateChanged;

	/** 获取 InitState 的静态 Tag（方便外部模块引用） */
	static FGameplayTag GetSpawnedTag()          { return FDGameplayTags::InitState_Spawned; }
	static FGameplayTag GetDataAvailableTag()    { return FDGameplayTags::InitState_DataAvailable; }
	static FGameplayTag GetDataInitializedTag()  { return FDGameplayTags::InitState_DataInitialized; }
	static FGameplayTag GetGameplayReadyTag()    { return FDGameplayTags::InitState_GameplayReady; }

protected:
	virtual void BeginPlay() override;

	/** 实现 IGameFrameworkInitStateInterface */
	virtual FName GetFeatureName() const override { return FName(TEXT("PawnExtension")); }
	virtual void CheckDefaultInitialization() override;

	/** 推进到指定状态 */
	void ContinueInitStateTransition(FGameplayTag NewState);

	/** 当前初始化阶段 */
	FGameplayTag CurrentInitState;
};
