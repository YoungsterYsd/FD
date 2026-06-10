// Copyright YoungSterYSD. All Rights Reserved.

#include "AsyncAction_ExperienceReady.h"
#include "FDExperienceManagerComponent.h"
#include "FDExperienceDefinition.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_ExperienceReady)

UAsyncAction_ExperienceReady* UAsyncAction_ExperienceReady::WaitForExperienceReady(UObject* WorldContextObject)
{
    UAsyncAction_ExperienceReady* Action = NewObject<UAsyncAction_ExperienceReady>();
    if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        if (AGameStateBase* GameState = World->GetGameState())
        {
            Action->ManagerComponent = GameState->FindComponentByClass<UFDExperienceManagerComponent>();
        }
    }
    return Action;
}

void UAsyncAction_ExperienceReady::Activate()
{
    if (ManagerComponent.IsValid())
    {
        if (ManagerComponent->IsExperienceLoaded())
        {
            // 已加载完毕，直接回调
            HandleExperienceLoaded(ManagerComponent->GetCurrentExperience());
        }
        else
        {
            // 尚未加载，注册委托等待
            ManagerComponent->OnExperienceLoaded.AddUObject(
                this, &UAsyncAction_ExperienceReady::HandleExperienceLoaded);
        }
    }
    else
    {
        // ManagerComponent 不存在，立即结束
        SetReadyToDestroy();
    }
}

void UAsyncAction_ExperienceReady::HandleExperienceLoaded(const UFDExperienceDefinition* Experience)
{
    OnReady.Broadcast(Experience);
    SetReadyToDestroy();
}
