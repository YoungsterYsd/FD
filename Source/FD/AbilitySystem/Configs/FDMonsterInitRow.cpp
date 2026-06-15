// Copyright YoungSterYSD. All Rights Reserved.

#include "FDMonsterInitRow.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/FDHealthSet.h"
#include "AbilitySystem/Attributes/FDCombatSet.h"
#include "AbilitySystem/Attributes/FDTenacitySet.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LogChannels/FDLogChannels.h"

bool FFDMonsterInitRow::ApplyTo(UAbilitySystemComponent* ASC, int32 MonsterID, AActor* Owner)
{
	if (!ASC)
	{
		return false;
	}

	UDataTable* Table = FFDTableMgrRowBase::GetOrLoadTable(TEXT("MonsterInit"));
	if (!Table)
	{
		return false;
	}

	const FName RowName(*FString::FromInt(MonsterID));
	const FFDMonsterInitRow* Row = Table->FindRow<FFDMonsterInitRow>(RowName, TEXT("MonsterInit"));
	if (!Row)
	{
		UE_LOG(LogFDGAS, Warning,
			TEXT("FFDMonsterInitRow::ApplyTo - Row not found for ID: %d"), MonsterID);
		return false;
	}

	UFDHealthSet* HS = const_cast<UFDHealthSet*>(ASC->GetSet<UFDHealthSet>());
	UFDCombatSet* CS = const_cast<UFDCombatSet*>(ASC->GetSet<UFDCombatSet>());
	UFDTenacitySet* TS = const_cast<UFDTenacitySet*>(ASC->GetSet<UFDTenacitySet>());

	if (HS)
	{
		HS->SetHpBasic(Row->HpBasic);
	}

	if (CS)
	{
		CS->SetAtkBasic(Row->AtkBasic);
		CS->SetDef(Row->Def);
		CS->SetAttackSpeed(Row->AttackSpeed);
		CS->SetCastSpeed(Row->AttackSpeed);
		CS->SetBodyStrength(Row->BodyStrength);
	}

	if (TS)
	{
		TS->SetTenacityMax(Row->TenacityMax);
		TS->SetTenacityCurrent(Row->TenacityCurrent);
	}

	if (Owner)
	{
		if (ACharacter* Char = Cast<ACharacter>(Owner))
		{
			Char->GetCharacterMovement()->MaxWalkSpeed = Row->MoveSpeed;
		}
	}

	UE_LOG(LogFDGAS, Log,
		TEXT("FFDMonsterInitRow::ApplyTo - Initialized %d (HP:%.0f, ATK:%.0f, DEF:%.0f, SPD:%.0f)"),
		MonsterID, Row->HpBasic, Row->AtkBasic, Row->Def, Row->MoveSpeed);

	return true;
}
