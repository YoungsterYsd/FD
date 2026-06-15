// Copyright YoungSterYSD. All Rights Reserved.

#include "FDCombatSet.h"
#include "Net/UnrealNetwork.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDCombatSet)

UFDCombatSet::UFDCombatSet()
	: AtkBasic(10.0f)
	, AtkMul(0.0f)
	, Atk(10.0f)
	, Def(0.0f)
	, DefIgn(0.0f)
	, Crit(0.05f)
	, CritDmg(1.5f)
	, DmgInc(0.0f)
	, DmgDec(0.0f)
	, AttackSpeed(1.0f)
	, CastSpeed(1.0f)
	, BodyStrength(0.0f)
{
}

void UFDCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UFDCombatSet, AtkBasic, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDCombatSet, AtkMul, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDCombatSet, Atk, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDCombatSet, Def, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDCombatSet, DefIgn, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDCombatSet, Crit, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDCombatSet, CritDmg, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDCombatSet, DmgInc, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDCombatSet, DmgDec, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDCombatSet, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDCombatSet, CastSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDCombatSet, BodyStrength, COND_None, REPNOTIFY_Always);
}

void UFDCombatSet::OnRep_AtkBasic(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDCombatSet, AtkBasic, OldValue);
}

void UFDCombatSet::OnRep_AtkMul(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDCombatSet, AtkMul, OldValue);
}

void UFDCombatSet::OnRep_Atk(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDCombatSet, Atk, OldValue);
}

void UFDCombatSet::OnRep_Def(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDCombatSet, Def, OldValue);
}

void UFDCombatSet::OnRep_DefIgn(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDCombatSet, DefIgn, OldValue);
}

void UFDCombatSet::OnRep_Crit(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDCombatSet, Crit, OldValue);
}

void UFDCombatSet::OnRep_CritDmg(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDCombatSet, CritDmg, OldValue);
}

void UFDCombatSet::OnRep_DmgInc(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDCombatSet, DmgInc, OldValue);
}

void UFDCombatSet::OnRep_DmgDec(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDCombatSet, DmgDec, OldValue);
}

void UFDCombatSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDCombatSet, AttackSpeed, OldValue);
}

void UFDCombatSet::OnRep_CastSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDCombatSet, CastSpeed, OldValue);
}

void UFDCombatSet::OnRep_BodyStrength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDCombatSet, BodyStrength, OldValue);
}

// ---- Tag-driven Damage Bonuses ---- //

float UFDCombatSet::GetDamageBonus(FGameplayTag BonusTag) const
{
	const float* Found = DamageBonuses.Find(BonusTag);
	return Found ? *Found : 0.0f;
}

void UFDCombatSet::SetDamageBonus(FGameplayTag BonusTag, float Value)
{
	DamageBonuses.Add(BonusTag, Value);
}

bool UFDCombatSet::HasDamageBonus(FGameplayTag BonusTag) const
{
	return DamageBonuses.Contains(BonusTag);
}

void UFDCombatSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetCritAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 1.0f);
	}
}

void UFDCombatSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetAtkBasicAttribute() || Attribute == GetAtkMulAttribute())
	{
		RecalculateAtk();
	}
}

void UFDCombatSet::RecalculateAtk()
{
	const float NewAtk = GetAtkBasic() * (1.0f + GetAtkMul());
	if (!FMath::IsNearlyEqual(GetAtk(), NewAtk))
	{
		UE_LOG(LogFDGAS, Verbose, TEXT("UFDCombatSet::RecalculateAtk - Atk: %.1f -> %.1f (Basic:%.1f, Mul:%.2f)"),
			GetAtk(), NewAtk, GetAtkBasic(), GetAtkMul());
		SetAtk(NewAtk);
	}
}
