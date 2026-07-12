// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FDHeroInitData.generated.h"

/**
 * Pure-data struct for hero character attribute initialization.
 * Extracted from FFDCharacterInitRow; no logic, no table registration.
 * Fields are BlueprintReadWrite for UnLua table mapping compatibility.
 */
USTRUCT(BlueprintType)
struct FFDHeroInitData
{
	GENERATED_BODY()

	/** Base health value. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Hero")
	float HpBasic = 100.0f;

	/** Health multiplier (percentage, 0.0 = +0%). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Hero")
	float HpMul = 0.0f;

	/** Base attack value. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Hero")
	float AtkBasic = 10.0f;

	/** Attack multiplier (percentage, 0.0 = +0%). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Hero")
	float AtkMul = 0.0f;

	/** Defense value (flat damage reduction). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Hero")
	float Def = 5.0f;

	/** Defense penetration (ignores this amount of target's defense). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Hero")
	float DefIgn = 0.0f;

	/** Critical hit rate, clamped to [0, 1]. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Hero")
	float Crit = 0.05f;

	/** Critical hit damage multiplier (e.g., 1.5 = +150%). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Hero")
	float CritDmg = 1.5f;

	/** Overall damage increase (positive percentage). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Hero")
	float DmgInc = 0.0f;

	/** Overall damage decrease (applied to incoming damage). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Hero")
	float DmgDec = 0.0f;

	/** Attack speed multiplier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Hero")
	float AttackSpeed = 1.0f;

	/** Casting speed multiplier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Hero")
	float CastSpeed = 1.0f;

	/** Base movement speed (will be set on CharacterMovementComponent). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Hero")
	float MoveSpeed = 600.0f;

	/** Body strength for passive tenacity/damage reduction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Hero")
	float BodyStrength = 0.0f;
};
