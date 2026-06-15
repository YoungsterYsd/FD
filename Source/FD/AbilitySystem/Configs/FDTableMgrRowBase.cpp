// Copyright YoungSterYSD. All Rights Reserved.

#include "FDTableMgrRowBase.h"
#include "AbilitySystem/Configs/FDTableMgr.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDTableMgrRowBase)

void FFDTableMgrRowBase::RegisterParser(const FName& Name, const FSoftObjectPath& Path)
{
	if (UFDTableMgr* Mgr = GetTableMgr())
	{
		Mgr->RegisterParser(Name, Path);
	}
}

UDataTable* FFDTableMgrRowBase::GetOrLoadTable(const FName& Name)
{
	if (UFDTableMgr* Mgr = GetTableMgr())
	{
		return Mgr->GetOrLoadTable(Name);
	}
	return nullptr;
}

UFDTableMgr* FFDTableMgrRowBase::GetTableMgr()
{
	if (GEngine)
	{
		for (const FWorldContext& Ctx : GEngine->GetWorldContexts())
		{
			if (UWorld* World = Ctx.World())
			{
				if (UGameInstance* GI = World->GetGameInstance())
				{
					return GI->GetSubsystem<UFDTableMgr>();
				}
			}
		}
	}
	return nullptr;
}
