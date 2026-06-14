// Copyright YoungSterYSD. All Rights Reserved.

#include "RPGTenacitySet.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGTenacitySet)

UFDRPGTenacitySet::UFDRPGTenacitySet()
	: TenacityCurrent(100.0f)
	, TenacityMax(100.0f)
	, TenacityAtkRate(1.0f)
	, BodyStrength(0.0f)
	, TenacityDamage(0.0f)
{
}

void UFDRPGTenacitySet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UFDRPGTenacitySet, TenacityCurrent, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDRPGTenacitySet, TenacityMax, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDRPGTenacitySet, TenacityAtkRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDRPGTenacitySet, BodyStrength, COND_None, REPNOTIFY_Always);
}

void UFDRPGTenacitySet::OnRep_TenacityCurrent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDRPGTenacitySet, TenacityCurrent, OldValue);
}

void UFDRPGTenacitySet::OnRep_TenacityMax(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDRPGTenacitySet, TenacityMax, OldValue);
}

void UFDRPGTenacitySet::OnRep_TenacityAtkRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDRPGTenacitySet, TenacityAtkRate, OldValue);
}

void UFDRPGTenacitySet::OnRep_BodyStrength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDRPGTenacitySet, BodyStrength, OldValue);
}

void UFDRPGTenacitySet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetTenacityCurrentAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetTenacityMax());
	}
}

void UFDRPGTenacitySet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetTenacityDamageAttribute())
	{
		const float TenacityBefore = GetTenacityCurrent();
		const float LocalTenacityDamage = GetTenacityDamage();
		const float NewTenacity = FMath::Max(0.0f, TenacityBefore - LocalTenacityDamage);
		SetTenacityCurrent(NewTenacity);
		SetTenacityDamage(0.0f);

		UE_LOG(LogFDGAS, Verbose, TEXT("UFDRPGTenacitySet::PostGameplayEffectExecute - TenacityDamage: %.1f, TenacityCurrent: %.1f -> %.1f"),
			LocalTenacityDamage, TenacityBefore, NewTenacity);

		if (GetTenacityCurrent() <= 0.0f)
		{
			OnTenacityZero.Broadcast(
				Data.EffectSpec.GetContext().GetInstigator(),
				Data.EffectSpec.GetContext().GetEffectCauser(),
				&Data.EffectSpec,
				LocalTenacityDamage,
				TenacityBefore,
				GetTenacityCurrent());
		}
	}
}
