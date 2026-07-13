// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FD : ModuleRules
{
	public FD(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayTags",
			"GameplayTasks",
			"GameplayAbilities",
			"ModularGameplay",
			"GameFeatures",
			"GameplayMessageRuntime",
			"UMG",
			"CommonUI",
			"CommonInput",
			"Niagara",
			"AIModule",
			"DataRegistry",
			"SignificanceManager",
			"NavigationSystem",
			"StructUtils",
			"Json",
			"JsonUtilities"
		});

		// FLuaFunction/FLuaTable 模板在头文件中直接调用 Lua C API，
		// 需要显式链接 Lua 导入库以解析 lua_gettop/lua_settop 等符号
		PrivateDependencyModuleNames.AddRange(new string[] {
			"UnLua",
			"Lua"
		});

		// 将模块根目录加入私有 include 路径，使子目录下的文件能以 "GameplayTags/FDGameplayTags.h" 形式引用模块内其他文件
		PrivateIncludePaths.Add(ModuleDirectory);

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
