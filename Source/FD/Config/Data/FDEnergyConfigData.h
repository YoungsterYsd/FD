// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FDEnergyConfigData.generated.h"

/**
 * Pure-data struct for energy pool configuration.
 * Extracted from FFDEnergyConfigRow; no logic, no table registration.
 * Fields are BlueprintReadWrite for UnLua table mapping compatibility.
 */
USTRUCT(BlueprintType)
struct FFDEnergyConfigData
{
	GENERATED_BODY()

	/** Energy type tag (e.g. Energy.Type.General, Energy.Type.Ultimate). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Energy")
	FGameplayTag EnergyType;

	/** Maximum energy pool capacity. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Energy")
	float MaxCapacity = 100.0f;

	/** Energy charge rate multiplier (1.0 = normal rate, 0.0 = no passive regen). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Energy")
	float ChargeRate = 1.0f;
};
