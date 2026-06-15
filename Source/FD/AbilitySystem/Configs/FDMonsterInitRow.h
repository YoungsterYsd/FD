// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Configs/FDTableMgrRowBase.h"
#include "FDMonsterInitRow.generated.h"

class UAbilitySystemComponent;

/**
 * DataTable row for monster attribute initialization.
 * Each row defines the base combat attributes for a monster type (Goblin, Elite, Boss, etc.).
 */
USTRUCT()
struct FFDMonsterInitRow : public FFDTableMgrRowBase
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

	/** Register this parser with UFDTableMgr at module startup. */
	static void Register()
	{
		FFDTableMgrRowBase::RegisterParser(TEXT("MonsterInit"), FSoftObjectPath(TEXT("/Game/Data/DT_MonsterInit")));
	}

	/**
	 * Apply monster init attributes to the given ASC by MonsterID.
	 * @param ASC - Target AbilitySystemComponent.
	 * @param MonsterID - Numeric monster type ID.
	 * @param Owner - Optional owner actor for MoveSpeed application.
	 * @return true if the row was found and attributes were applied.
	 */
	static bool ApplyTo(UAbilitySystemComponent* ASC, int32 MonsterID, AActor* Owner = nullptr);
};
