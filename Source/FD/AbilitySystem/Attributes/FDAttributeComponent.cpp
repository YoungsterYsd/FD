// Copyright YoungSterYSD. All Rights Reserved.

#include "FDAttributeComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/FDEnergySet.h"
#include "AbilitySystem/Attributes/FDHealthSet.h"
#include "AbilitySystem/Attributes/FDCombatSet.h"
#include "AbilitySystem/Attributes/FDTenacitySet.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDAttributeComponent)

UFDAttributeComponent::UFDAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFDAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

UAbilitySystemComponent* UFDAttributeComponent::GetASC() const
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}

	UAbilitySystemComponent* FoundASC = Owner->FindComponentByClass<UAbilitySystemComponent>();
	if (!FoundASC)
	{
		if (const APawn* Pawn = Cast<APawn>(Owner))
		{
			if (const APlayerState* PS = Pawn->GetPlayerState())
			{
				FoundASC = PS->FindComponentByClass<UAbilitySystemComponent>();
			}
		}
	}

	return FoundASC;
}

// ---- Health ---- //

float UFDAttributeComponent::GetHpPercent() const
{
	UAbilitySystemComponent* LocalASC = GetASC();
	if (!LocalASC)
	{
		return 1.0f;
	}

	const UFDHealthSet* HS = LocalASC->GetSet<UFDHealthSet>();
	if (!HS || HS->GetHpMax() <= 0.0f)
	{
		return 1.0f;
	}

	return HS->GetHpCurrent() / HS->GetHpMax();
}

bool UFDAttributeComponent::IsAlive() const
{
	return GetHpPercent() > 0.0f;
}

// ---- Energy ---- //

float UFDAttributeComponent::GetEnergyPercent(FGameplayTag EnergyType) const
{
	UAbilitySystemComponent* LocalASC = GetASC();
	if (!LocalASC)
	{
		return 0.0f;
	}

	const UFDEnergySet* ES = LocalASC->GetSet<UFDEnergySet>();
	if (!ES)
	{
		return 0.0f;
	}

	return ES->GetEnergyPercent(EnergyType);
}

bool UFDAttributeComponent::HasEnoughEnergy(FGameplayTag EnergyType, float Amount) const
{
	UAbilitySystemComponent* LocalASC = GetASC();
	if (!LocalASC)
	{
		return false;
	}

	const UFDEnergySet* ES = LocalASC->GetSet<UFDEnergySet>();
	if (!ES)
	{
		return false;
	}

	return ES->HasEnoughEnergy(EnergyType, Amount);
}

void UFDAttributeComponent::ConsumeEnergy(FGameplayTag EnergyType, float Amount)
{
	UAbilitySystemComponent* LocalASC = GetASC();
	if (!LocalASC)
	{
		return;
	}

	UFDEnergySet* ES = const_cast<UFDEnergySet*>(LocalASC->GetSet<UFDEnergySet>());
	if (ES)
	{
		ES->ConsumeEnergy(EnergyType, Amount);
	}
}

// ---- Tenacity ---- //

bool UFDAttributeComponent::IsTenacityBroken() const
{
	UAbilitySystemComponent* LocalASC = GetASC();
	if (!LocalASC)
	{
		return false;
	}

	const UFDTenacitySet* TS = LocalASC->GetSet<UFDTenacitySet>();
	return TS && TS->GetTenacityCurrent() <= 0.0f;
}

float UFDAttributeComponent::GetTenacityPercent() const
{
	UAbilitySystemComponent* LocalASC = GetASC();
	if (!LocalASC)
	{
		return 1.0f;
	}

	const UFDTenacitySet* TS = LocalASC->GetSet<UFDTenacitySet>();
	if (!TS || TS->GetTenacityMax() <= 0.0f)
	{
		return 1.0f;
	}

	return TS->GetTenacityCurrent() / TS->GetTenacityMax();
}

// ---- Damage Bonus ---- //

float UFDAttributeComponent::GetDamageBonus(FGameplayTag DamageCategory) const
{
	UAbilitySystemComponent* LocalASC = GetASC();
	if (!LocalASC)
	{
		return 0.0f;
	}

	const UFDCombatSet* CS = LocalASC->GetSet<UFDCombatSet>();
	if (!CS)
	{
		return 0.0f;
	}

	return CS->GetDamageBonus(DamageCategory);
}
