// Copyright YoungSterYSD. All Rights Reserved.

#include "FDGamePawnExtensionComponent.h"
#include "GameplayTags/FDGameplayTags.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDGamePawnExtensionComponent)

UFDGamePawnExtensionComponent::UFDGamePawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentInitState = FDGameplayTags::InitState_Spawned;
}

void UFDGamePawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// 自动推进到 DataAvailable —— 此时 Pawn 已生成，Controller 可能已就位
	if (CurrentInitState == FDGameplayTags::InitState_Spawned)
	{
		CheckDefaultInitialization();
	}
}

bool UFDGamePawnExtensionComponent::HasReachedInitState(FGameplayTag DesiredState) const
{
	// 简单的前后比较：如果 DesiredState 在 InitState 顺序中排在 CurrentInitState 之前或相等，则认为已到达
	static const TArray<FGameplayTag> InitOrder = {
		FDGameplayTags::InitState_Spawned,
		FDGameplayTags::InitState_DataAvailable,
		FDGameplayTags::InitState_DataInitialized,
		FDGameplayTags::InitState_GameplayReady
	};

	const int32 CurrentIdx = InitOrder.IndexOfByKey(CurrentInitState);
	const int32 DesiredIdx = InitOrder.IndexOfByKey(DesiredState);

	return CurrentIdx >= DesiredIdx;
}

void UFDGamePawnExtensionComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> TransitionOrder = {
		FDGameplayTags::InitState_Spawned,
		FDGameplayTags::InitState_DataAvailable,
		FDGameplayTags::InitState_DataInitialized,
		FDGameplayTags::InitState_GameplayReady
	};

	const int32 CurrentIdx = TransitionOrder.IndexOfByKey(CurrentInitState);
	if (CurrentIdx < TransitionOrder.Num() - 1)
	{
		ContinueInitStateTransition(TransitionOrder[CurrentIdx + 1]);
	}
}

void UFDGamePawnExtensionComponent::ContinueInitStateTransition(FGameplayTag NewState)
{
	CurrentInitState = NewState;
	UE_LOG(LogFD, Log, TEXT("PawnExtension InitState: %s"), *NewState.ToString());

	// 通知订阅者
	OnInitStateChanged.Broadcast(GetOwner(), NewState);
}
