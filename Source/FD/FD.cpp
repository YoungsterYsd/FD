// Copyright Epic Games, Inc. All Rights Reserved.

#include "FD.h"
#include "Modules/ModuleManager.h"
#include "AbilitySystem/Executions/FDEnergyCostExecution.h"

void FFDModule::StartupModule()
{
	UE_LOG(LogFD, Log, TEXT("FD Module Started"));
}

void FFDModule::ShutdownModule()
{
	UE_LOG(LogFD, Log, TEXT("FD Module Shutdown"));
}

IMPLEMENT_PRIMARY_GAME_MODULE(FFDModule, FD, "FD");
