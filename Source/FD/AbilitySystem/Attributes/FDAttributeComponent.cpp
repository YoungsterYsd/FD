// Copyright YoungSterYSD. All Rights Reserved.

#include "FDAttributeComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/RPGEnergySet.h"
#include "AbilitySystem/Attributes/RPGHealthSet.h"
#include "AbilitySystem/Attributes/RPGCombatSet.h"
#include "AbilitySystem/Attributes/RPGTenacitySet.h"
#include "AbilitySystem/Configs/FDEnergyConfigRow.h"
#include "AbilitySystem/Configs/FDCharacterInitRow.h"
#include "AbilitySystem/Configs/FDMonsterInitRow.h"
#include "AbilitySystem/Configs/FDDamageBonusConfigRow.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "LogChannels/FDLogChannels.h"

UFDAttributeComponent::UFDAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFDAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// Attempt to find ASC directly on the owner first
	ASC = Owner->FindComponentByClass<UAbilitySystemComponent>();

	// Fallback: ASC may be on PlayerState
	if (!ASC)
	{
		if (const APawn* Pawn = Cast<APawn>(Owner))
		{
			if (const APlayerState* PS = Pawn->GetPlayerState())
			{
				ASC = PS->FindComponentByClass<UAbilitySystemComponent>();
			}
		}
	}

	if (ASC)
	{
		// C1 验收: 自动授予 4 个 AttributeSet (C3 阶段将改为 AbilitySet 管理)
		if (!ASC->GetSet<UFDRPGHealthSet>())
			ASC->AddSet<UFDRPGHealthSet>();
		if (!ASC->GetSet<UFDRPGCombatSet>())
			ASC->AddSet<UFDRPGCombatSet>();
		if (!ASC->GetSet<UFDRPGTenacitySet>())
			ASC->AddSet<UFDRPGTenacitySet>();
		if (!ASC->GetSet<UFDRPGEnergySet>())
			ASC->AddSet<UFDRPGEnergySet>();

		EnergySet = const_cast<UFDRPGEnergySet*>(ASC->GetSet<UFDRPGEnergySet>());
	}

	InitializeEnergyPools();
	InitializeDamageBonuses();

	// Initialize combat attributes from DataTables
	if (bIsMonster)
	{
		InitializeMonsterAttributes(CharacterID);
	}
	else
	{
		InitializeCharacterAttributes(CharacterID);
	}
}

void UFDAttributeComponent::InitializeEnergyPools()
{
	if (!EnergySet || !EnergyConfigTable)
	{
		return;
	}

	static const FString Context(TEXT("FDAttributeComponent_EnergyConfig"));
	TArray<FFDEnergyConfigRow*> Rows;
	EnergyConfigTable->GetAllRows(Context, Rows);

	for (const FFDEnergyConfigRow* Row : Rows)
	{
		if (Row->EnergyType.IsValid())
		{
			EnergySet->RegisterEnergyPool(Row->EnergyType, Row->InitialMax, Row->ChargeRate);
		}
	}

	UE_LOG(LogFDGAS, Log, TEXT("UFDAttributeComponent::InitializeEnergyPools - Registered %d energy pools from DataTable."),
		Rows.Num());
}

// ---- Health ---- //

float UFDAttributeComponent::GetHpPercent() const
{
	if (!ASC)
	{
		return 1.0f;
	}

	const UFDRPGHealthSet* HS = ASC->GetSet<UFDRPGHealthSet>();
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
	if (!EnergySet)
	{
		return 0.0f;
	}

	return EnergySet->GetEnergyPercent(EnergyType);
}

bool UFDAttributeComponent::HasEnoughEnergy(FGameplayTag EnergyType, float Amount) const
{
	if (!EnergySet)
	{
		return false;
	}

	return EnergySet->HasEnoughEnergy(EnergyType, Amount);
}

void UFDAttributeComponent::ConsumeEnergy(FGameplayTag EnergyType, float Amount)
{
	if (EnergySet)
	{
		EnergySet->ConsumeEnergy(EnergyType, Amount);
	}
}

// ---- Tenacity ---- //

bool UFDAttributeComponent::IsTenacityBroken() const
{
	if (!ASC)
	{
		return false;
	}

	const UFDRPGTenacitySet* TS = ASC->GetSet<UFDRPGTenacitySet>();
	return TS && TS->GetTenacityCurrent() <= 0.0f;
}

float UFDAttributeComponent::GetTenacityPercent() const
{
	if (!ASC)
	{
		return 1.0f;
	}

	const UFDRPGTenacitySet* TS = ASC->GetSet<UFDRPGTenacitySet>();
	if (!TS || TS->GetTenacityMax() <= 0.0f)
	{
		return 1.0f;
	}

	return TS->GetTenacityCurrent() / TS->GetTenacityMax();
}

// ---- Damage Bonus ---- //

float UFDAttributeComponent::GetDamageBonus(FGameplayTag DamageCategory) const
{
	if (!ASC)
	{
		return 0.0f;
	}

	const UFDRPGCombatSet* CS = ASC->GetSet<UFDRPGCombatSet>();
	if (!CS)
	{
		return 0.0f;
	}

	return CS->GetDamageBonus(DamageCategory);
}

// ---- Attribute Initialization ---- //

void UFDAttributeComponent::InitializeCharacterAttributes(const int32 ID)
{
	if (!ASC || !CharacterInitTable)
	{
		return;
	}

	const FName RowName(*FString::FromInt(ID));
	const FFDCharacterInitRow* Row = CharacterInitTable->FindRow<FFDCharacterInitRow>(RowName, TEXT("AttrInit"));
	if (!Row)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("UFDAttributeComponent::InitializeCharacterAttributes - Row not found for ID: %d"), ID);
		return;
	}

	UFDRPGHealthSet* HS = const_cast<UFDRPGHealthSet*>(ASC->GetSet<UFDRPGHealthSet>());
	UFDRPGCombatSet* CS = const_cast<UFDRPGCombatSet*>(ASC->GetSet<UFDRPGCombatSet>());

	if (HS)
	{
		HS->SetHpBasic(Row->HpBasic);
		HS->SetHpMul(Row->HpMul);
		// HpMax/HpCurrent are auto-derived by PostAttributeChange
	}

	if (CS)
	{
		CS->SetAtkBasic(Row->AtkBasic);
		CS->SetAtkMul(Row->AtkMul);
		CS->SetDef(Row->Def);
		CS->SetDefIgn(Row->DefIgn);
		CS->SetCrit(Row->Crit);
		CS->SetCritDmg(Row->CritDmg);
		CS->SetDmgInc(Row->DmgInc);
		CS->SetDmgDec(Row->DmgDec);
		CS->SetAttackSpeed(Row->AttackSpeed);
		CS->SetCastSpeed(Row->CastSpeed);
		CS->SetBodyStrength(Row->BodyStrength);
	}

	// MoveSpeed is written directly to CharacterMovementComponent
	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		Char->GetCharacterMovement()->MaxWalkSpeed = Row->MoveSpeed;
	}

	UE_LOG(LogFDGAS, Log, TEXT("UFDAttributeComponent::InitializeCharacterAttributes - Initialized %d (HP:%.0f, ATK:%.0f, DEF:%.0f, SPD:%.0f)"),
		ID, Row->HpBasic, Row->AtkBasic, Row->Def, Row->MoveSpeed);
}

void UFDAttributeComponent::InitializeMonsterAttributes(const int32 ID)
{
	if (!ASC || !MonsterInitTable)
	{
		return;
	}

	const FName RowName(*FString::FromInt(ID));
	const FFDMonsterInitRow* Row = MonsterInitTable->FindRow<FFDMonsterInitRow>(RowName, TEXT("AttrInit"));
	if (!Row)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("UFDAttributeComponent::InitializeMonsterAttributes - Row not found for ID: %d"), ID);
		return;
	}

	UFDRPGHealthSet* HS = const_cast<UFDRPGHealthSet*>(ASC->GetSet<UFDRPGHealthSet>());
	UFDRPGCombatSet* CS = const_cast<UFDRPGCombatSet*>(ASC->GetSet<UFDRPGCombatSet>());
	UFDRPGTenacitySet* TS = const_cast<UFDRPGTenacitySet*>(ASC->GetSet<UFDRPGTenacitySet>());

	if (HS)
	{
		HS->SetHpBasic(Row->HpBasic);
	}

	if (CS)
	{
		CS->SetAtkBasic(Row->AtkBasic);
		CS->SetDef(Row->Def);
		CS->SetAttackSpeed(Row->AttackSpeed);
		CS->SetCastSpeed(Row->AttackSpeed);  // Monsters use AttackSpeed for both
		CS->SetBodyStrength(Row->BodyStrength);
	}

	if (TS)
	{
		TS->SetTenacityMax(Row->TenacityMax);
		TS->SetTenacityCurrent(Row->TenacityCurrent);
	}

	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		Char->GetCharacterMovement()->MaxWalkSpeed = Row->MoveSpeed;
	}

	UE_LOG(LogFDGAS, Log, TEXT("UFDAttributeComponent::InitializeMonsterAttributes - Initialized %d (HP:%.0f, ATK:%.0f, DEF:%.0f, SPD:%.0f)"),
		ID, Row->HpBasic, Row->AtkBasic, Row->Def, Row->MoveSpeed);
}

void UFDAttributeComponent::InitializeDamageBonuses()
{
	if (!ASC || !DamageBonusConfigTable)
	{
		return;
	}

	UFDRPGCombatSet* CS = const_cast<UFDRPGCombatSet*>(ASC->GetSet<UFDRPGCombatSet>());
	if (!CS)
	{
		return;
	}

	static const FString Context(TEXT("FDAttributeComponent_DamageBonusConfig"));
	TArray<FDDamageBonusConfigRow*> Rows;
	DamageBonusConfigTable->GetAllRows(Context, Rows);

	int32 BonusCount = 0;
	for (const FDDamageBonusConfigRow* Row : Rows)
	{
		if (Row->BonusTag.IsValid())
		{
			CS->SetDamageBonus(Row->BonusTag, Row->DefaultValue);
			++BonusCount;
		}
	}

	UE_LOG(LogFDGAS, Log, TEXT("UFDAttributeComponent::InitializeDamageBonuses - Registered %d damage bonus types globally."),
		BonusCount);
}
