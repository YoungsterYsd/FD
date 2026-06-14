// Copyright YoungSterYSD. All Rights Reserved.

#include "RPGHealthSet.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGHealthSet)

UFDRPGHealthSet::UFDRPGHealthSet()
	: HpBasic(100.0f)
	, HpMul(0.0f)
	, HpMax(100.0f)
	, HpCurrent(100.0f)
	, Healing(0.0f)
	, Damage(0.0f)
{
}

void UFDRPGHealthSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UFDRPGHealthSet, HpBasic, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDRPGHealthSet, HpMul, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDRPGHealthSet, HpMax, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDRPGHealthSet, HpCurrent, COND_None, REPNOTIFY_Always);
}

void UFDRPGHealthSet::OnRep_HpBasic(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDRPGHealthSet, HpBasic, OldValue);
}

void UFDRPGHealthSet::OnRep_HpMul(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDRPGHealthSet, HpMul, OldValue);
}

void UFDRPGHealthSet::OnRep_HpMax(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDRPGHealthSet, HpMax, OldValue);
}

void UFDRPGHealthSet::OnRep_HpCurrent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDRPGHealthSet, HpCurrent, OldValue);
}

void UFDRPGHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHpCurrentAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetHpMax());
	}
}

void UFDRPGHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const float HpBefore = GetHpCurrent();
	float LocalDamage = 0.0f;
	float LocalHealing = 0.0f;

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		LocalDamage = GetDamage();
		const float NewHp = FMath::Clamp(HpBefore - LocalDamage, 0.0f, GetHpMax());
		SetHpCurrent(NewHp);
		SetDamage(0.0f);

		UE_LOG(LogFDGAS, Verbose, TEXT("UFDRPGHealthSet::PostGameplayEffectExecute - Damage: %.1f, HpCurrent: %.1f -> %.1f"),
			LocalDamage, HpBefore, NewHp);

		if (GetHpCurrent() <= 0.0f)
		{
			OnOutOfHealth.Broadcast(
				Data.EffectSpec.GetContext().GetInstigator(),
				Data.EffectSpec.GetContext().GetEffectCauser(),
				&Data.EffectSpec,
				LocalDamage,
				HpBefore,
				GetHpCurrent());
		}
	}

	if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		LocalHealing = GetHealing();
		const float NewHp = FMath::Clamp(HpBefore + LocalHealing, 0.0f, GetHpMax());
		SetHpCurrent(NewHp);
		SetHealing(0.0f);

		UE_LOG(LogFDGAS, Verbose, TEXT("UFDRPGHealthSet::PostGameplayEffectExecute - Healing: %.1f, HpCurrent: %.1f -> %.1f"),
			LocalHealing, HpBefore, NewHp);
	}
}

void UFDRPGHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetHpBasicAttribute() || Attribute == GetHpMulAttribute())
	{
		RecalculateHpMax();
	}
	else if (Attribute == GetHpMaxAttribute())
	{
		// When max health increases, leave current health unchanged.
		// When max health decreases, clamp current health to new max.
		if (GetHpCurrent() > NewValue)
		{
			SetHpCurrent(NewValue);
		}
	}
}

void UFDRPGHealthSet::RecalculateHpMax()
{
	const float NewHpMax = GetHpBasic() * (1.0f + GetHpMul());
	if (!FMath::IsNearlyEqual(GetHpMax(), NewHpMax))
	{
		UE_LOG(LogFDGAS, Verbose, TEXT("UFDRPGHealthSet::RecalculateHpMax - HpMax: %.1f -> %.1f (Basic:%.1f, Mul:%.2f)"),
			GetHpMax(), NewHpMax, GetHpBasic(), GetHpMul());
		SetHpMax(NewHpMax);
	}
}
