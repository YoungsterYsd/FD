// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Attributes/FDAttributeSet.h"
#include "GameplayTagContainer.h"
#include "RPGEnergySet.generated.h"

/**
 * Single energy pool entry.
 *
 * ChargeRate is server-only and not replicated.
 * Current and Max are replicated via NetSerialize.
 */
USTRUCT(BlueprintType)
struct FEnergyPoolEntry
{
	GENERATED_BODY()

	/** Energy type tag (e.g. Energy.Type.General, Energy.Type.Ultimate). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag EnergyType;

	/** Current energy value, clamped to [0, Max]. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Current = 0.0f;

	/** Maximum energy capacity. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Max = 100.0f;

	/**
	 * Energy charge rate multiplier (1.0 = normal rate).
	 * Server-only, NOT replicated to clients.
	 */
	float ChargeRate = 1.0f;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	bool operator==(const FEnergyPoolEntry& Other) const
	{
		return EnergyType == Other.EnergyType;
	}
};

template<>
struct TStructOpsTypeTraits<FEnergyPoolEntry> : public TStructOpsTypeTraitsBase2<FEnergyPoolEntry>
{
	enum
	{
		WithNetSerializer = true,
	};
};

/**
 * FD RPG Energy AttributeSet (Multi-Pool Tag Management).
 *
 * Supports arbitrary energy types via FGameplayTag. New energy types only
 * require adding a tag, no C++ changes.
 *
 * Energy operations are called directly by the Cost system, not through
 * GameplayEffect Modifiers.
 *
 * - RegisterEnergyPool: Called at initialization (player/monster).
 * - ConsumeEnergy / AddEnergy: Called by Cost / regeneration systems.
 * - OnEnergyDepleted: Broadcast when any pool reaches zero.
 * - OnEnergyChanged: Broadcast when any pool changes.
 */
UCLASS(BlueprintType)
class FD_API UFDRPGEnergySet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UFDRPGEnergySet();

	// ---- Pool Management ----

	/**
	 * Register a new energy pool.
	 * @param EnergyType - Tag identifying the energy type.
	 * @param InitialMax - Initial maximum capacity.
	 * @param ChargeRate - Recharge rate multiplier (1.0 = normal rate).
	 */
	UFUNCTION(BlueprintCallable, Category = "FD|Energy")
	void RegisterEnergyPool(FGameplayTag EnergyType, float InitialMax, float ChargeRate = 1.0f);

	/**
	 * Remove an energy pool by type.
	 * @param EnergyType - Tag identifying the energy type to remove.
	 */
	UFUNCTION(BlueprintCallable, Category = "FD|Energy")
	void UnregisterEnergyPool(FGameplayTag EnergyType);

	// ---- Query ----

	/** Get current energy for the given type. Returns 0.0f if pool not found. */
	UFUNCTION(BlueprintCallable, Category = "FD|Energy")
	float GetEnergyCurrent(FGameplayTag EnergyType) const;

	/** Get max energy for the given type. Returns 0.0f if pool not found. */
	UFUNCTION(BlueprintCallable, Category = "FD|Energy")
	float GetEnergyMax(FGameplayTag EnergyType) const;

	/** Get charge rate for the given type. Returns 0.0f if pool not found. */
	UFUNCTION(BlueprintCallable, Category = "FD|Energy")
	float GetEnergyChargeRate(FGameplayTag EnergyType) const;

	/** Check if enough energy is available for the given cost. */
	UFUNCTION(BlueprintCallable, Category = "FD|Energy")
	bool HasEnoughEnergy(FGameplayTag EnergyType, float Amount) const;

	/** Get energy percentage (Current / Max) for the given type. Returns 0.0f if pool not found. */
	UFUNCTION(BlueprintCallable, Category = "FD|Energy")
	float GetEnergyPercent(FGameplayTag EnergyType) const;

	/** Check if a pool exists for the given type. */
	UFUNCTION(BlueprintCallable, Category = "FD|Energy")
	bool HasEnergyPool(FGameplayTag EnergyType) const;

	// ---- Operations ----

	/**
	 * Consume energy from the pool. Clamped to [0, Max].
	 * Called by the Cost system. Must be called on server.
	 */
	UFUNCTION(BlueprintCallable, Category = "FD|Energy")
	void ConsumeEnergy(FGameplayTag EnergyType, float Amount);

	/**
	 * Add energy to the pool (regeneration / GE / item). Clamped to [0, Max].
	 * Called by regeneration tick or external systems. Must be called on server.
	 */
	UFUNCTION(BlueprintCallable, Category = "FD|Energy")
	void AddEnergy(FGameplayTag EnergyType, float Amount);

	// ---- Events ----

	/** Broadcast when any energy pool reaches zero after consumption. */
	mutable FFDAttributeEvent OnEnergyDepleted;

	/** Broadcast when any energy pool value changes. */
	mutable FFDAttributeEvent OnEnergyChanged;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	/** Find a const pointer to the pool entry matching the given tag. */
	const FEnergyPoolEntry* FindPoolEntry(FGameplayTag EnergyType) const;

	/** Find a mutable pointer to the pool entry matching the given tag. */
	FEnergyPoolEntry* FindPoolEntryMutable(FGameplayTag EnergyType);

	/** Clamp the pool entry's Current value to [0, Max]. */
	void ClampPool(FEnergyPoolEntry& Entry);

	/**
	 * Replicated array of energy pools.
	 * Each entry's EnergyType, Current, and Max are replicated.
	 * ChargeRate is server-only.
	 */
	UPROPERTY(Replicated)
	TArray<FEnergyPoolEntry> EnergyPools;
};
