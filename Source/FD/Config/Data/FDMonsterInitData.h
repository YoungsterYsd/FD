// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FDMonsterInitData.generated.h"

/**
 * Pure-data struct for monster attribute initialization.
 * Extracted from FFDMonsterInitRow; no logic, no table registration.
 * Fields are BlueprintReadWrite for UnLua table mapping compatibility.
 */
USTRUCT(BlueprintType)
struct FFDMonsterInitData
{
	GENERATED_BODY()

	/** Base health value. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Monster")
	float HpBasic = 100.0f;

	/** Base attack value. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Monster")
	float AtkBasic = 10.0f;

	/** Defense value (flat damage reduction). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Monster")
	float Def = 5.0f;

	/** Base movement speed (will be set on CharacterMovementComponent). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Monster")
	float MoveSpeed = 400.0f;

	/** Attack speed multiplier (also used as CastSpeed for monsters). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Monster")
	float AttackSpeed = 1.0f;

	/** Body strength for passive tenacity/damage reduction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Monster")
	float BodyStrength = 0.0f;

	/** Maximum tenacity capacity. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Monster")
	float TenacityMax = 100.0f;

	/** Current tenacity at spawn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Monster")
	float TenacityCurrent = 100.0f;
};
