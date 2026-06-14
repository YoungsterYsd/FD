// Copyright YoungSterYSD. All Rights Reserved.

#include "RPGCombatSet.h"
#include "Net/UnrealNetwork.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGCombatSet)

UFDRPGCombatSet::UFDRPGCombatSet()
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

void UFDRPGCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UFDRPGCombatSet, AtkBasic, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDRPGCombatSet, AtkMul, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDRPGCombatSet, Atk, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDRPGCombatSet, Def, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDRPGCombatSet, DefIgn, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDRPGCombatSet, Crit, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDRPGCombatSet, CritDmg, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDRPGCombatSet, DmgInc, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDRPGCombatSet, DmgDec, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDRPGCombatSet, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDRPGCombatSet, CastSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDRPGCombatSet, BodyStrength, COND_None, REPNOTIFY_Always);
}

void UFDRPGCombatSet::OnRep_AtkBasic(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDRPGCombatSet, AtkBasic, OldValue);
}

void UFDRPGCombatSet::OnRep_AtkMul(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDRPGCombatSet, AtkMul, OldValue);
}

void UFDRPGCombatSet::OnRep_Atk(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDRPGCombatSet, Atk, OldValue);
}

void UFDRPGCombatSet::OnRep_Def(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDRPGCombatSet, Def, OldValue);
}

void UFDRPGCombatSet::OnRep_DefIgn(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDRPGCombatSet, DefIgn, OldValue);
}

void UFDRPGCombatSet::OnRep_Crit(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDRPGCombatSet, Crit, OldValue);
}

void UFDRPGCombatSet::OnRep_CritDmg(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDRPGCombatSet, CritDmg, OldValue);
}

void UFDRPGCombatSet::OnRep_DmgInc(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDRPGCombatSet, DmgInc, OldValue);
}

void UFDRPGCombatSet::OnRep_DmgDec(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDRPGCombatSet, DmgDec, OldValue);
}

void UFDRPGCombatSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDRPGCombatSet, AttackSpeed, OldValue);
}

void UFDRPGCombatSet::OnRep_CastSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDRPGCombatSet, CastSpeed, OldValue);
}

void UFDRPGCombatSet::OnRep_BodyStrength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDRPGCombatSet, BodyStrength, OldValue);
}

// ---- Tag-driven Damage Bonuses ---- //

float UFDRPGCombatSet::GetDamageBonus(FGameplayTag BonusTag) const
{
	const float* Found = DamageBonuses.Find(BonusTag);
	return Found ? *Found : 0.0f;
}

void UFDRPGCombatSet::SetDamageBonus(FGameplayTag BonusTag, float Value)
{
	DamageBonuses.Add(BonusTag, Value);
}

bool UFDRPGCombatSet::HasDamageBonus(FGameplayTag BonusTag) const
{
	return DamageBonuses.Contains(BonusTag);
}

void UFDRPGCombatSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetCritAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 1.0f);
	}
}

void UFDRPGCombatSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetAtkBasicAttribute() || Attribute == GetAtkMulAttribute())
	{
		RecalculateAtk();
	}
}

void UFDRPGCombatSet::RecalculateAtk()
{
	const float NewAtk = GetAtkBasic() * (1.0f + GetAtkMul());
	if (!FMath::IsNearlyEqual(GetAtk(), NewAtk))
	{
		UE_LOG(LogFDGAS, Verbose, TEXT("UFDRPGCombatSet::RecalculateAtk - Atk: %.1f -> %.1f (Basic:%.1f, Mul:%.2f)"),
			GetAtk(), NewAtk, GetAtkBasic(), GetAtkMul());
		SetAtk(NewAtk);
	}
}
