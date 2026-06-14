// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FDMonsterInitRow.generated.h"

/**
 * DataTable row for monster attribute initialization.
 * Each row defines the base combat attributes for a monster type (Goblin, Elite, Boss, etc.).
 * Used by UFDAttributeComponent::InitializeMonsterAttributes at BeginPlay.
 */
USTRUCT()
struct FFDMonsterInitRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Base health value. */
	UPROPERTY(EditAnywhere)
	float HpBasic = 100.0f;

	/** Base attack value. */
	UPROPERTY(EditAnywhere)
	float AtkBasic = 10.0f;

	/** Defense value (flat damage reduction). */
	UPROPERTY(EditAnywhere)
	float Def = 5.0f;

	/** Base movement speed (will be set on CharacterMovementComponent). */
	UPROPERTY(EditAnywhere)
	float MoveSpeed = 400.0f;

	/** Attack speed multiplier (also used as CastSpeed for monsters). */
	UPROPERTY(EditAnywhere)
	float AttackSpeed = 1.0f;

	/** Body strength for passive tenacity/damage reduction. */
	UPROPERTY(EditAnywhere)
	float BodyStrength = 0.0f;

	/** Maximum tenacity capacity. */
	UPROPERTY(EditAnywhere)
	float TenacityMax = 100.0f;

	/** Current tenacity at spawn. */
	UPROPERTY(EditAnywhere)
	float TenacityCurrent = 100.0f;
};
