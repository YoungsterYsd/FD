// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "FDEnergyConfigRow.generated.h"

/**
 * DataTable row for energy pool configuration.
 * Each row defines an energy type, its initial max value, and charge rate.
 * Used by UFDAttributeComponent to register energy pools from CSV at runtime.
 */
USTRUCT()
struct FFDEnergyConfigRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Energy type tag (e.g. Energy.Type.General, Energy.Type.Ultimate). */
	UPROPERTY(EditAnywhere)
	FGameplayTag EnergyType;

	/** Initial maximum energy capacity. */
	UPROPERTY(EditAnywhere)
	float InitialMax = 100.0f;

	/** Energy charge rate multiplier (1.0 = normal rate, 0.0 = no passive regen). */
	UPROPERTY(EditAnywhere)
	float ChargeRate = 1.0f;
};
