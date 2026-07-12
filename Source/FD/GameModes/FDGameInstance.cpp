// Copyright YoungSterYSD. All Rights Reserved.

#include "FDGameInstance.h"
#include "LogChannels/FDLogChannels.h"
#include "UnLua.h"

void UFDGameInstance::Init()
{
    Super::Init();
    UE_LOG(LogFDGAS, Log, TEXT("[FDConfig] UFDGameInstance::Init — loading configs from Lua"));
    LoadConfigsFromLua();
}

void UFDGameInstance::Shutdown()
{
    UE_LOG(LogFDGAS, Log, TEXT("[FDConfig] UFDGameInstance::Shutdown"));
    Super::Shutdown();
}

void UFDGameInstance::LoadConfigsFromLua()
{
    UnLua::FLuaEnv Env;

    // Step 1: 获取 require 函数
    const auto Require = UnLua::FLuaFunction(&Env, "_G", "require");

    // Step 2: require Bootstrap.ConfigLoader 模块
    const auto RetValues1 = Require.Call("Bootstrap.ConfigLoader");
    if (RetValues1.Num() == 0)
    {
        UE_LOG(LogFDGAS, Error, TEXT("[FDConfig] Failed to require Bootstrap.ConfigLoader module"));
        return;
    }

    // Step 3: 获取模块 table
    const auto LuaTable = UnLua::FLuaTable(&Env, RetValues1[0]);

    // Step 4: 调用 LoadConfigs(GameInstance)
    const auto RetValues2 = LuaTable.Call("LoadConfigs", this);

    if (RetValues2.Num() > 0)
    {
        const bool bSuccess = RetValues2[0].Value<bool>();
        UE_LOG(LogFDGAS, Log, TEXT("[FDConfig] LoadConfigsFromLua result: %s"),
            bSuccess ? TEXT("SUCCESS") : TEXT("FAILED"));
    }
    else
    {
        UE_LOG(LogFDGAS, Warning, TEXT("[FDConfig] LoadConfigsFromLua: no return value"));
    }
}
