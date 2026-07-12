// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FDGameInstance.generated.h"

UCLASS()
class FD_API UFDGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;
    virtual void Shutdown() override;

private:
    /** 通过 UnLua API 调用 Bootstrap/ConfigLoader.lua 中的 LoadConfigs() */
    void LoadConfigsFromLua();
};
