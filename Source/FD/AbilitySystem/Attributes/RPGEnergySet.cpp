// Copyright YoungSterYSD. All Rights Reserved.

#include "RPGEnergySet.h"
#include "Net/UnrealNetwork.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGEnergySet)

// ---- FEnergyPoolEntry NetSerialize ----

bool FEnergyPoolEntry::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << EnergyType;
	Ar << Current;
	Ar << Max;
	// ChargeRate is server-only, NOT serialized for network
	bOutSuccess = true;
	return true;
}

// ---- UFDRPGEnergySet ----

UFDRPGEnergySet::UFDRPGEnergySet()
{
}

void UFDRPGEnergySet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UFDRPGEnergySet, EnergyPools, COND_None);
}

// ---- Pool Management ----

void UFDRPGEnergySet::RegisterEnergyPool(FGameplayTag EnergyType, float InitialMax, float ChargeRate)
{
	if (!EnergyType.IsValid())
	{
		UE_LOG(LogFDGAS, Warning, TEXT("UFDRPGEnergySet::RegisterEnergyPool - Invalid EnergyType tag!"));
		return;
	}

	if (InitialMax <= 0.0f)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("UFDRPGEnergySet::RegisterEnergyPool - InitialMax must be > 0! Tag: %s, InitialMax: %.1f"),
			*EnergyType.ToString(), InitialMax);
		return;
	}

	// Check if pool already exists
	FEnergyPoolEntry* Existing = FindPoolEntryMutable(EnergyType);
	if (Existing)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("UFDRPGEnergySet::RegisterEnergyPool - Pool already exists for Tag: %s. Updating Max and ChargeRate."),
			*EnergyType.ToString());
		Existing->Max = InitialMax;
		Existing->ChargeRate = ChargeRate;
		ClampPool(*Existing);
		return;
	}

	FEnergyPoolEntry NewEntry;
	NewEntry.EnergyType = EnergyType;
	NewEntry.Max = InitialMax;
	NewEntry.Current = 0.f;  // 初始能量为 0, 通过战斗/时间累积
	NewEntry.ChargeRate = ChargeRate;

	EnergyPools.Add(NewEntry);

	UE_LOG(LogFDGAS, Log, TEXT("UFDRPGEnergySet::RegisterEnergyPool - Registered pool: %s, Max: %.1f, ChargeRate: %.2f"),
		*EnergyType.ToString(), InitialMax, ChargeRate);
}

void UFDRPGEnergySet::UnregisterEnergyPool(FGameplayTag EnergyType)
{
	const int32 Index = EnergyPools.IndexOfByPredicate([&EnergyType](const FEnergyPoolEntry& Entry)
	{
		return Entry.EnergyType == EnergyType;
	});

	if (Index != INDEX_NONE)
	{
		UE_LOG(LogFDGAS, Log, TEXT("UFDRPGEnergySet::UnregisterEnergyPool - Removing pool: %s"),
			*EnergyType.ToString());
		EnergyPools.RemoveAt(Index);
	}
}

// ---- Query ----

float UFDRPGEnergySet::GetEnergyCurrent(FGameplayTag EnergyType) const
{
	const FEnergyPoolEntry* Entry = FindPoolEntry(EnergyType);
	return Entry ? Entry->Current : 0.0f;
}

float UFDRPGEnergySet::GetEnergyMax(FGameplayTag EnergyType) const
{
	const FEnergyPoolEntry* Entry = FindPoolEntry(EnergyType);
	return Entry ? Entry->Max : 0.0f;
}

float UFDRPGEnergySet::GetEnergyChargeRate(FGameplayTag EnergyType) const
{
	const FEnergyPoolEntry* Entry = FindPoolEntry(EnergyType);
	return Entry ? Entry->ChargeRate : 0.0f;
}

bool UFDRPGEnergySet::HasEnoughEnergy(FGameplayTag EnergyType, float Amount) const
{
	const FEnergyPoolEntry* Entry = FindPoolEntry(EnergyType);
	if (!Entry)
	{
		return false;
	}
	return Entry->Current >= Amount;
}

float UFDRPGEnergySet::GetEnergyPercent(FGameplayTag EnergyType) const
{
	const FEnergyPoolEntry* Entry = FindPoolEntry(EnergyType);
	if (!Entry || Entry->Max <= 0.0f)
	{
		return 0.0f;
	}
	return FMath::Clamp(Entry->Current / Entry->Max, 0.0f, 1.0f);
}

bool UFDRPGEnergySet::HasEnergyPool(FGameplayTag EnergyType) const
{
	return FindPoolEntry(EnergyType) != nullptr;
}

// ---- Operations ----

void UFDRPGEnergySet::ConsumeEnergy(FGameplayTag EnergyType, float Amount)
{
	FEnergyPoolEntry* Entry = FindPoolEntryMutable(EnergyType);
	if (!Entry)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("UFDRPGEnergySet::ConsumeEnergy - No pool found for Tag: %s"),
			*EnergyType.ToString());
		return;
	}

	const float OldValue = Entry->Current;
	Entry->Current = FMath::Max(0.0f, Entry->Current - Amount);

	UE_LOG(LogFDGAS, Verbose, TEXT("UFDRPGEnergySet::ConsumeEnergy - Tag: %s, Amount: %.1f, %.1f -> %.1f"),
		*EnergyType.ToString(), Amount, OldValue, Entry->Current);

	// Broadcast change event
	OnEnergyChanged.Broadcast(
		nullptr,  // EffectInstigator
		nullptr,  // EffectCauser
		nullptr,  // EffectSpec
		Amount,   // EffectMagnitude
		OldValue,
		Entry->Current);

	// Broadcast depleted event if zero
	if (Entry->Current <= 0.0f)
	{
		UE_LOG(LogFDGAS, Log, TEXT("UFDRPGEnergySet::ConsumeEnergy - Pool depleted! Tag: %s"),
			*EnergyType.ToString());

		OnEnergyDepleted.Broadcast(
			nullptr,
			nullptr,
			nullptr,
			Amount,
			OldValue,
			Entry->Current);
	}
}

void UFDRPGEnergySet::AddEnergy(FGameplayTag EnergyType, float Amount)
{
	FEnergyPoolEntry* Entry = FindPoolEntryMutable(EnergyType);
	if (!Entry)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("UFDRPGEnergySet::AddEnergy - No pool found for Tag: %s"),
			*EnergyType.ToString());
		return;
	}

	const float OldValue = Entry->Current;
	Entry->Current = FMath::Min(Entry->Max, Entry->Current + Amount);

	UE_LOG(LogFDGAS, Verbose, TEXT("UFDRPGEnergySet::AddEnergy - Tag: %s, Amount: %.1f, %.1f -> %.1f"),
		*EnergyType.ToString(), Amount, OldValue, Entry->Current);

	// Broadcast change event
	OnEnergyChanged.Broadcast(
		nullptr,
		nullptr,
		nullptr,
		Amount,
		OldValue,
		Entry->Current);
}

// ---- Private Helpers ----

const FEnergyPoolEntry* UFDRPGEnergySet::FindPoolEntry(FGameplayTag EnergyType) const
{
	for (const FEnergyPoolEntry& Entry : EnergyPools)
	{
		if (Entry.EnergyType == EnergyType)
		{
			return &Entry;
		}
	}
	return nullptr;
}

FEnergyPoolEntry* UFDRPGEnergySet::FindPoolEntryMutable(FGameplayTag EnergyType)
{
	for (FEnergyPoolEntry& Entry : EnergyPools)
	{
		if (Entry.EnergyType == EnergyType)
	{
		return &Entry;
	}
	}
	return nullptr;
}

void UFDRPGEnergySet::ClampPool(FEnergyPoolEntry& Entry)
{
	Entry.Current = FMath::Clamp(Entry.Current, 0.0f, Entry.Max);
}
