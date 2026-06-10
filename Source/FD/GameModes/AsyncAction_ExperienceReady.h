// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_ExperienceReady.generated.h"

class UFDExperienceManagerComponent;
class UFDExperienceDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnExperienceReadyDelegate, const UFDExperienceDefinition*, Experience);

/**
 * 蓝图异步节点：等待 Experience 加载完成。
 *
 * 使用方式：
 *   在蓝图中调用 WaitForExperienceReady 节点，
 *   绑定 OnReady 引脚，Experience 加载完毕后自动触发。
 */
UCLASS()
class FD_API UAsyncAction_ExperienceReady : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    /**
     * 等待当前关卡的 Experience 加载完成。
     * @param WorldContextObject - 世界上下文对象
     * @return 异步 Action 实例
     */
    UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"))
    static UAsyncAction_ExperienceReady* WaitForExperienceReady(UObject* WorldContextObject);

    /** Experience 加载完成时触发 */
    UPROPERTY(BlueprintAssignable)
    FOnExperienceReadyDelegate OnReady;

protected:
    virtual void Activate() override;

    UFUNCTION()
    void HandleExperienceLoaded(const UFDExperienceDefinition* Experience);

    /** 弱引用 ExperienceManagerComponent，避免悬空指针 */
    TWeakObjectPtr<UFDExperienceManagerComponent> ManagerComponent;
};
