// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Configs/FDTableMgrRowBase.h"
#include "FDEnergyConfigRow.generated.h"

class UFDEnergySet;

/**
 * DataTable row for energy pool configuration.
 * Each row defines an energy type, its max capacity, and charge rate.
 */
USTRUCT()
struct FFDEnergyConfigRow : public FFDTableMgrRowBase
{
	GENERATED_BODY()

	/** Energy type tag (e.g. Energy.Type.General, Energy.Type.Ultimate). */
	UPROPERTY(EditAnywhere)
	FGameplayTag EnergyType;

	/** Maximum energy pool capacity. */
	UPROPERTY(EditAnywhere)
	float MaxCapacity = 100.0f;

	/** Energy charge rate multiplier (1.0 = normal rate, 0.0 = no passive regen). */
	UPROPERTY(EditAnywhere)
	float ChargeRate = 1.0f;

	/** Register this parser with UFDTableMgr at module startup. */
	static void Register()
	{
		FFDTableMgrRowBase::RegisterParser(TEXT("EnergyConfig"), FSoftObjectPath(TEXT("/Game/Data/DT_EnergyConfig")));
	}

	/** Read DT_EnergyConfig and register all energy pools on the given EnergySet. */
	static void ApplyTo(UFDEnergySet* Target);
};
