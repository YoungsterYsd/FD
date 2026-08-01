// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "ActiveGameplayEffectHandle.h"
#include "FDAbilitySet.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UAttributeSet;
class UAbilitySystemComponent;

/**
 * Single ability entry in an AbilitySet.
 * Includes the GA class, level, and optional InputTag for automatic input binding.
 */
USTRUCT(BlueprintType)
struct FFDAbilitySet_GameplayAbility
{
	GENERATED_BODY()

	/** The GameplayAbility subclass to grant. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> Ability;

	/** Level at which to grant this ability. */
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	/** Input tag to bind this ability to (e.g. InputTag.Ability.Q). */
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * Single GameplayEffect entry in an AbilitySet.
 */
USTRUCT(BlueprintType)
struct FFDAbilitySet_GameplayEffect
{
	GENERATED_BODY()

	/** The GameplayEffect class to apply. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect;

	/** Level at which to apply this effect. */
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

/**
 * Single AttributeSet entry in an AbilitySet.
 */
USTRUCT(BlueprintType)
struct FFDAbilitySet_AttributeSet
{
	GENERATED_BODY()

	/** The AttributeSet subclass to add to the ASC. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;
};

/**
 * Handle collection returned by GiveToAbilitySystem.
 * Tracks all granted abilities, effects, and attribute sets for later removal.
 */
USTRUCT(BlueprintType)
struct FFDAbilitySet_GrantedHandles
{
	GENERATED_BODY()

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);

	/** Remove all granted items from the given ASC. */
	void TakeFromAbilitySystem(UAbilitySystemComponent* ASC);

	/** Granted ability spec handles. */
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	/** Active gameplay effect handles. */
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	/** Granted attribute set references. */
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};

/**
 * AbilitySet — A DataAsset that bundles abilities, effects, and attribute sets.
 *
 * Replaces the hardcoded AddSet<>() pattern from C1.
 * Configured per-character in Blueprint and granted at runtime.
 * AttributeSets are granted BEFORE abilities (GAS requirement).
 */
UCLASS(BlueprintType, Const)
class FD_API UFDAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * Grant all abilities, effects, and attribute sets to an ASC.
	 * @param ASC - Target AbilitySystemComponent.
	 * @param OutHandles - Optional output handles for later removal.
	 * @param SourceObject - Optional source object for ability specs.
	 */
	void GiveToAbilitySystem(
		UAbilitySystemComponent* ASC,
		FFDAbilitySet_GrantedHandles* OutHandles = nullptr,
		UObject* SourceObject = nullptr) const;

	/**
	 * Remove all previously granted items from an ASC.
	 * @param ASC - Target AbilitySystemComponent.
	 * @param InHandles - Handles returned from GiveToAbilitySystem.
	 */
	void RemoveFromAbilitySystem(
		UAbilitySystemComponent* ASC,
		FFDAbilitySet_GrantedHandles* InHandles) const;

protected:
	/** Abilities to grant. */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<FFDAbilitySet_GameplayAbility> GrantedAbilities;

	/** GameplayEffects to apply on grant. */
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TArray<FFDAbilitySet_GameplayEffect> GrantedEffects;

	/** AttributeSets to add. */
	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	TArray<FFDAbilitySet_AttributeSet> GrantedAttributes;
};
