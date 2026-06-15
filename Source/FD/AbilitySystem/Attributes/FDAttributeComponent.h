// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "FDAttributeComponent.generated.h"

class UAbilitySystemComponent;

/**
 * Attribute query facade.
 *
 * Mounted on AFDCharacter, this component provides a unified attribute query API
 * (HP%, Energy%, Tenacity, DamageBonus) that delegates to individual AttributeSets.
 * Attribute initialization is handled by row struct static methods (FFDCharacterInitRow::ApplyTo, etc.)
 * called from experience/bootstrap code rather than this component.
 */
UCLASS(BlueprintType)
class FD_API UFDAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFDAttributeComponent();

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
	 * @param DamageCategory - GameplayTag identifying the damage type.
	 */
	UFUNCTION(BlueprintCallable, Category = "FD|Attribute")
	float GetDamageBonus(FGameplayTag DamageCategory) const;

protected:
	virtual void BeginPlay() override;

private:
	/** Dynamically find ASC from owner or PlayerState. Not cached. */
	UAbilitySystemComponent* GetASC() const;
};
