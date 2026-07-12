// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FDDamageBonusConfigData.generated.h"

/**
 * Pure-data struct for global damage bonus type registration.
 * Extracted from FFDDamageBonusConfigRow; no logic, no table registration.
 * Fields are BlueprintReadWrite for UnLua table mapping compatibility.
 */
USTRUCT(BlueprintType)
struct FFDDamageBonusConfigData
{
	GENERATED_BODY()

	/** Damage bonus tag (e.g. Bonus.NormalAtk, Bonus.SpecialAtk, Bonus.ElementFire). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|DamageBonus")
	FGameplayTag BonusTag;

	/** Default bonus multiplier (0.0 = +0%, added to base damage multiplier). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|DamageBonus")
	float DefaultValue = 0.0f;
};
