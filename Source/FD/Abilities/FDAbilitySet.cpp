// Copyright YoungSterYSD. All Rights Reserved.

#include "FDAbilitySet.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "AbilitySystem/Attributes/FDEnergySet.h"
#include "AbilitySystem/Attributes/FDCombatSet.h"
#include "Config/UFDConfigSubsystem.h"
#include "Config/Data/FDEnergyConfigData.h"
#include "Config/Data/FDDamageBonusConfigData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDAbilitySet)

void FFDAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FFDAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FFDAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	if (Set)
	{
		GrantedAttributeSets.Add(Set);
	}
}

void FFDAbilitySet_GrantedHandles::TakeFromAbilitySystem(UAbilitySystemComponent* ASC)
{
	if (!ASC)
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			ASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

void UFDAbilitySet::GiveToAbilitySystem(
	UAbilitySystemComponent* ASC,
	FFDAbilitySet_GrantedHandles* OutHandles,
	UObject* SourceObject) const
{
	if (!ASC)
	{
		return;
	}

	FFDAbilitySet_GrantedHandles TempHandles;

	// Step 1: Grant AttributeSets FIRST (GAS requires sets exist before abilities)
	for (const FFDAbilitySet_AttributeSet& Entry : GrantedAttributes)
	{
		if (!Entry.AttributeSet)
		{
			continue;
		}

		// Check if an AttributeSet of this class already exists
		UAttributeSet* Set = nullptr;
		for (UAttributeSet* ExistingSet : ASC->GetSpawnedAttributes())
		{
			if (ExistingSet && ExistingSet->IsA(Entry.AttributeSet))
			{
				Set = ExistingSet;
				break;
			}
		}

		// Create and add if not already present
		if (!Set)
		{
			Set = NewObject<UAttributeSet>(ASC->GetOwner(), Entry.AttributeSet);
			ASC->AddAttributeSetSubobject(Set);
		}

		if (OutHandles)
		{
			OutHandles->AddAttributeSet(Set);
		}
		else
		{
			TempHandles.AddAttributeSet(Set);
		}
	}

	// Step 1.5: Initialize Energy pools + Damage bonuses from ConfigSubsystem (Lua-driven)
	if (const UFDEnergySet* ES = ASC->GetSet<UFDEnergySet>())
	{
		if (UFDConfigSubsystem* Config = UFDConfigSubsystem::Get(ASC->GetOwner()))
		{
			Config->ForEachConfigRow<FFDEnergyConfigData>(
				[ES](int32 RowID, const FFDEnergyConfigData& Data)
				{
					const_cast<UFDEnergySet*>(ES)->RegisterEnergyPool(
						Data.EnergyType, Data.MaxCapacity, Data.ChargeRate);
					return true;  // continue iteration
				});
		}
	}
	if (const UFDCombatSet* CS = ASC->GetSet<UFDCombatSet>())
	{
		if (UFDConfigSubsystem* Config = UFDConfigSubsystem::Get(ASC->GetOwner()))
		{
			Config->ForEachConfigRow<FFDDamageBonusConfigData>(
				[CS](int32 RowID, const FFDDamageBonusConfigData& Data)
				{
					const_cast<UFDCombatSet*>(CS)->SetDamageBonus(
						Data.BonusTag, Data.DefaultValue);
					return true;  // continue iteration
				});
		}
	}

	// Step 2: Apply GameplayEffects
	for (const FFDAbilitySet_GameplayEffect& Entry : GrantedEffects)
	{
		if (!Entry.GameplayEffect)
		{
			continue;
		}

		UGameplayEffect* EffectCDO = Entry.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle EffectHandle = ASC->ApplyGameplayEffectToSelf(
			EffectCDO, Entry.EffectLevel, ASC->MakeEffectContext());

		if (OutHandles)
		{
			OutHandles->AddGameplayEffectHandle(EffectHandle);
		}
		else
		{
			TempHandles.AddGameplayEffectHandle(EffectHandle);
		}
	}

	// Step 3: Grant Abilities
	for (const FFDAbilitySet_GameplayAbility& Entry : GrantedAbilities)
	{
		if (!Entry.Ability)
		{
			continue;
		}

		FGameplayAbilitySpec Spec(Entry.Ability, Entry.AbilityLevel, INDEX_NONE, SourceObject);
		if (Entry.InputTag.IsValid())
		{
			Spec.DynamicAbilityTags.AddTag(Entry.InputTag);
		}

		const FGameplayAbilitySpecHandle AbilityHandle = ASC->GiveAbility(Spec);

		if (OutHandles)
		{
			OutHandles->AddAbilitySpecHandle(AbilityHandle);
		}
		else
		{
			TempHandles.AddAbilitySpecHandle(AbilityHandle);
		}
	}
}

void UFDAbilitySet::RemoveFromAbilitySystem(
	UAbilitySystemComponent* ASC,
	FFDAbilitySet_GrantedHandles* InHandles) const
{
	if (!ASC || !InHandles)
	{
		return;
	}

	InHandles->TakeFromAbilitySystem(ASC);
}
