// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "FDAttributeComponent.generated.h"

class UDataTable;
class UFDRPGEnergySet;
class UAbilitySystemComponent;

/**
 * Attribute aggregation component.
 *
 * Mounted on AFDCharacter, this component:
 * - Reads DT_EnergyConfig at BeginPlay and registers energy pools via UFDRPGEnergySet.
 * - Provides unified attribute query API (HP%, Energy%, Tenacity, DamageBonus).
 * - Delegates to individual AttributeSets (HealthSet, EnergySet, CombatSet, TenacitySet).
 */
UCLASS(BlueprintType)
class FD_API UFDAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFDAttributeComponent();

	/** Energy pool configuration DataTable (Row Struct: FFDEnergyConfigRow). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FD|Config")
	TObjectPtr<UDataTable> EnergyConfigTable;

	/** Character attribute initialization DataTable (Row Struct: FFDCharacterInitRow). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FD|Config")
	TObjectPtr<UDataTable> CharacterInitTable;

	/** Monster attribute initialization DataTable (Row Struct: FFDMonsterInitRow). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FD|Config")
	TObjectPtr<UDataTable> MonsterInitTable;

	/** Damage bonus type registration DataTable (Row Struct: FDDamageBonusConfigRow). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FD|Config")
	TObjectPtr<UDataTable> DamageBonusConfigTable;

	/** Character archetype ID (numeric, e.g. 1001). Used to look up init rows. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FD|Config")
	int32 CharacterID = 0;

	/** If true, this component initializes monster attributes instead of character attributes. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FD|Config")
	bool bIsMonster = false;

	// ---- Health ---- //

	/** Get current HP percentage in [0, 1]. Returns 1.0 if ASC or HealthSet unavailable. */
	UFUNCTION(BlueprintCallable, Category = "FD|Attribute")
	float GetHpPercent() const;

	/** Check if the owning character is alive (HpCurrent > 0). */
	UFUNCTION(BlueprintCallable, Category = "FD|Attribute")
	bool IsAlive() const;

	// ---- Energy ---- //

	/** Get energy percentage (Current / Max) for the given energy type. Returns 0.0 if pool not found. */
	UFUNCTION(BlueprintCallable, Category = "FD|Attribute")
	float GetEnergyPercent(FGameplayTag EnergyType) const;

	/** Check if enough energy is available for the given cost. */
	UFUNCTION(BlueprintCallable, Category = "FD|Attribute")
	bool HasEnoughEnergy(FGameplayTag EnergyType, float Amount) const;

	/** Consume energy from the pool. Must be called on server. */
	UFUNCTION(BlueprintCallable, Category = "FD|Attribute")
	void ConsumeEnergy(FGameplayTag EnergyType, float Amount);

	// ---- Tenacity ---- //

	/** Check if tenacity is broken (TenacityCurrent <= 0). */
	UFUNCTION(BlueprintCallable, Category = "FD|Attribute")
	bool IsTenacityBroken() const;

	/** Get tenacity percentage in [0, 1]. Returns 1.0 if TenacitySet unavailable. */
	UFUNCTION(BlueprintCallable, Category = "FD|Attribute")
	float GetTenacityPercent() const;

	// ---- Damage Bonus ---- //

	/**
	 * Get damage multiplier for a given damage category.
	 * TODO: Add GameplayTag matching when TAG_Ability_Attack_Normal etc. are defined.
	 */
	UFUNCTION(BlueprintCallable, Category = "FD|Attribute")
	float GetDamageBonus(FGameplayTag DamageCategory) const;

protected:
	virtual void BeginPlay() override;

private:
	/** Read DT rows and register energy pools via EnergySet. */
	void InitializeEnergyPools();

	/** Initialize character combat attributes from CharacterInitTable. */
	void InitializeCharacterAttributes(const int32 ID);

	/** Initialize monster combat attributes from MonsterInitTable. */
	void InitializeMonsterAttributes(const int32 ID);

	/** Read DT rows and register damage bonus types globally via CombatSet (mirrors InitializeEnergyPools). */
	void InitializeDamageBonuses();

	/** Cached ASC reference (from owner or owner's PlayerState). */
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	/** Cached EnergySet reference. */
	UPROPERTY()
	TObjectPtr<UFDRPGEnergySet> EnergySet;
};
