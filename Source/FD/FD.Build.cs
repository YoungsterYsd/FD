// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FD : ModuleRules
{
	public FD(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// 仅引用 UnLua 头文件（UnLua.h / FLuaEnv / FLuaFunction / FLuaTable），
		// 不通过模块依赖链接，避免运行时 UnLua 模块被加载两次。
		// 改为手动指定包含路径 + 导入库。
		PublicIncludePathModuleNames.AddRange(new string[] {
			"UnLua"
		});

		// 手动链接 UnLua 导入库，解析 FLuaEnv/FLuaFunction/FLuaTable 等符号
		// FD 是游戏模块，PluginDirectory 不可用，用 ModuleDirectory 相对路径
		string UnLuaLibPath = System.IO.Path.Combine(
			ModuleDirectory, "..", "..", "Plugins", "UnLua",
			"Intermediate", "Build", "Win64", "x64", "UnrealEditor",
			Target.Configuration.ToString(), "UnLua", "UnrealEditor-UnLua.lib");
		PublicAdditionalLibraries.Add(UnLuaLibPath);
	
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
