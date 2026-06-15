// Copyright YoungSterYSD. All Rights Reserved.

#include "FDCharacterInitRow.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/FDHealthSet.h"
#include "AbilitySystem/Attributes/FDCombatSet.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LogChannels/FDLogChannels.h"

bool FFDCharacterInitRow::ApplyTo(UAbilitySystemComponent* ASC, int32 CharacterID, AActor* Owner)
{
	if (!ASC)
	{
		return false;
	}

	UDataTable* Table = FFDTableMgrRowBase::GetOrLoadTable(TEXT("CharacterInit"));
	if (!Table)
	{
		return false;
	}

	const FName RowName(*FString::FromInt(CharacterID));
	const FFDCharacterInitRow* Row = Table->FindRow<FFDCharacterInitRow>(RowName, TEXT("CharInit"));
	if (!Row)
	{
		UE_LOG(LogFDGAS, Warning,
			TEXT("FFDCharacterInitRow::ApplyTo - Row not found for ID: %d"), CharacterID);
		return false;
	}

	UFDHealthSet* HS = const_cast<UFDHealthSet*>(ASC->GetSet<UFDHealthSet>());
	UFDCombatSet* CS = const_cast<UFDCombatSet*>(ASC->GetSet<UFDCombatSet>());

	if (HS)
	{
		HS->SetHpBasic(Row->HpBasic);
		HS->SetHpMul(Row->HpMul);
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

	if (Owner)
	{
		if (ACharacter* Char = Cast<ACharacter>(Owner))
		{
			Char->GetCharacterMovement()->MaxWalkSpeed = Row->MoveSpeed;
		}
	}

	UE_LOG(LogFDGAS, Log,
		TEXT("FFDCharacterInitRow::ApplyTo - Initialized %d (HP:%.0f, ATK:%.0f, DEF:%.0f, SPD:%.0f)"),
		CharacterID, Row->HpBasic, Row->AtkBasic, Row->Def, Row->MoveSpeed);

	return true;
}
