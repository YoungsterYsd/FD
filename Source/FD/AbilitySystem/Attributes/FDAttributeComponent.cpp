// Copyright YoungSterYSD. All Rights Reserved.

#include "FDAttributeComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/FDEnergySet.h"
#include "AbilitySystem/Attributes/FDHealthSet.h"
#include "AbilitySystem/Attributes/FDCombatSet.h"
#include "AbilitySystem/Attributes/FDTenacitySet.h"
#include "Config/UFDConfigSubsystem.h"
#include "Config/Data/FDHeroInitData.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	if (CharacterID == 0)
	{
		return;
	}

	UFDConfigSubsystem* Config = UFDConfigSubsystem::Get(GetOwner());
	if (!Config)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("[FDConfig] FDAttributeComponent::BeginPlay - ConfigSubsystem not available for CharacterID=%d"), CharacterID);
		return;
	}

	const FFDHeroInitData* Data = Config->GetConfigRow<FFDHeroInitData>(CharacterID);
	if (!Data)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("[FDConfig] FDAttributeComponent::BeginPlay - HeroInit data not found for ID=%d"), CharacterID);
		return;
	}

	UAbilitySystemComponent* ASC = GetASC();
	if (!ASC)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("[FDConfig] FDAttributeComponent::BeginPlay - ASC not found for CharacterID=%d"), CharacterID);
		return;
	}

	// Apply to HealthSet
	if (UFDHealthSet* HS = const_cast<UFDHealthSet*>(ASC->GetSet<UFDHealthSet>()))
	{
		HS->SetHpBasic(Data->HpBasic);
		HS->SetHpMul(Data->HpMul);
	}

	// Apply to CombatSet
	if (UFDCombatSet* CS = const_cast<UFDCombatSet*>(ASC->GetSet<UFDCombatSet>()))
	{
		CS->SetAtkBasic(Data->AtkBasic);
		CS->SetAtkMul(Data->AtkMul);
		CS->SetDef(Data->Def);
		CS->SetDefIgn(Data->DefIgn);
		CS->SetCrit(Data->Crit);
		CS->SetCritDmg(Data->CritDmg);
		CS->SetDmgInc(Data->DmgInc);
		CS->SetDmgDec(Data->DmgDec);
		CS->SetAttackSpeed(Data->AttackSpeed);
		CS->SetCastSpeed(Data->CastSpeed);
		CS->SetBodyStrength(Data->BodyStrength);
	}

	// Apply MoveSpeed to CharacterMovement
	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		Char->GetCharacterMovement()->MaxWalkSpeed = Data->MoveSpeed;
	}

	UE_LOG(LogFDGAS, Log, TEXT("[FDConfig] FDAttributeComponent - Initialized CharacterID=%d (HP=%.0f, ATK=%.0f, DEF=%.0f, SPD=%.0f)"),
		CharacterID, Data->HpBasic, Data->AtkBasic, Data->Def, Data->MoveSpeed);
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
