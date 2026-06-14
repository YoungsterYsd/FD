// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FDCharacterInitRow.generated.h"

/**
 * DataTable row for character attribute initialization.
 * Each row defines the base combat attributes for a character archetype (Warrior, Mage, Archer, etc.).
 * Used by UFDAttributeComponent::InitializeCharacterAttributes at BeginPlay.
 */
USTRUCT()
struct FFDCharacterInitRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Base health value. */
	UPROPERTY(EditAnywhere)
	float HpBasic = 100.0f;

	/** Health multiplier (percentage, 0.0 = +0%). */
	UPROPERTY(EditAnywhere)
	float HpMul = 0.0f;

	/** Base attack value. */
	UPROPERTY(EditAnywhere)
	float AtkBasic = 10.0f;

	/** Attack multiplier (percentage, 0.0 = +0%). */
	UPROPERTY(EditAnywhere)
	float AtkMul = 0.0f;

	/** Defense value (flat damage reduction). */
	UPROPERTY(EditAnywhere)
	float Def = 5.0f;

	/** Defense penetration (ignores this amount of target's defense). */
	UPROPERTY(EditAnywhere)
	float DefIgn = 0.0f;

	/** Critical hit rate, clamped to [0, 1]. */
	UPROPERTY(EditAnywhere)
	float Crit = 0.05f;

	/** Critical hit damage multiplier (e.g., 1.5 = +150%). */
	UPROPERTY(EditAnywhere)
	float CritDmg = 1.5f;

	/** Overall damage increase (positive percentage). */
	UPROPERTY(EditAnywhere)
	float DmgInc = 0.0f;

	/** Overall damage decrease (applied to incoming damage). */
	UPROPERTY(EditAnywhere)
	float DmgDec = 0.0f;

	/** Attack speed multiplier. */
	UPROPERTY(EditAnywhere)
	float AttackSpeed = 1.0f;

	/** Casting speed multiplier. */
	UPROPERTY(EditAnywhere)
	float CastSpeed = 1.0f;

	/** Base movement speed (will be set on CharacterMovementComponent). */
	UPROPERTY(EditAnywhere)
	float MoveSpeed = 600.0f;

	/** Body strength for passive tenacity/damage reduction. */
	UPROPERTY(EditAnywhere)
	float BodyStrength = 0.0f;
};
