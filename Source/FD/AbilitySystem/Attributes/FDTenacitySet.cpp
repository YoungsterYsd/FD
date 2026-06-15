// Copyright YoungSterYSD. All Rights Reserved.

#include "FDTenacitySet.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDTenacitySet)

UFDTenacitySet::UFDTenacitySet()
	: TenacityCurrent(100.0f)
	, TenacityMax(100.0f)
	, TenacityAtkRate(1.0f)
	, BodyStrength(0.0f)
	, TenacityDamage(0.0f)
{
}

void UFDTenacitySet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UFDTenacitySet, TenacityCurrent, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDTenacitySet, TenacityMax, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDTenacitySet, TenacityAtkRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFDTenacitySet, BodyStrength, COND_None, REPNOTIFY_Always);
}

void UFDTenacitySet::OnRep_TenacityCurrent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDTenacitySet, TenacityCurrent, OldValue);
}

void UFDTenacitySet::OnRep_TenacityMax(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDTenacitySet, TenacityMax, OldValue);
}

void UFDTenacitySet::OnRep_TenacityAtkRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDTenacitySet, TenacityAtkRate, OldValue);
}

void UFDTenacitySet::OnRep_BodyStrength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFDTenacitySet, BodyStrength, OldValue);
}

void UFDTenacitySet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetTenacityCurrentAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetTenacityMax());
	}
}

void UFDTenacitySet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetTenacityDamageAttribute())
	{
		const float TenacityBefore = GetTenacityCurrent();
		const float LocalTenacityDamage = GetTenacityDamage();
		const float NewTenacity = FMath::Max(0.0f, TenacityBefore - LocalTenacityDamage);
		SetTenacityCurrent(NewTenacity);
		SetTenacityDamage(0.0f);

		UE_LOG(LogFDGAS, Verbose, TEXT("UFDTenacitySet::PostGameplayEffectExecute - TenacityDamage: %.1f, TenacityCurrent: %.1f -> %.1f"),
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
