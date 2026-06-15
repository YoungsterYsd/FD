// Copyright YoungSterYSD. All Rights Reserved.

#include "FDEnergySet.h"
#include "Net/UnrealNetwork.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDEnergySet)

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

// ---- UFDEnergySet ----

UFDEnergySet::UFDEnergySet()
{
}

void UFDEnergySet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UFDEnergySet, EnergyPools, COND_None);
}

// ---- Pool Management ----

void UFDEnergySet::RegisterEnergyPool(FGameplayTag EnergyType, float InMaxCapacity, float ChargeRate)
{
	if (!EnergyType.IsValid())
	{
		UE_LOG(LogFDGAS, Warning, TEXT("UFDEnergySet::RegisterEnergyPool - Invalid EnergyType tag!"));
		return;
	}

	if (InMaxCapacity <= 0.0f)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("UFDEnergySet::RegisterEnergyPool - InMaxCapacity must be > 0! Tag: %s, MaxCapacity: %.1f"),
			*EnergyType.ToString(), InMaxCapacity);
		return;
	}

	// Check if pool already exists
	FEnergyPoolEntry* Existing = FindPoolEntryMutable(EnergyType);
	if (Existing)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("UFDEnergySet::RegisterEnergyPool - Pool already exists for Tag: %s. Updating Max and ChargeRate."),
			*EnergyType.ToString());
		Existing->Max = InMaxCapacity;
		Existing->ChargeRate = ChargeRate;
		ClampPool(*Existing);
		return;
	}

	FEnergyPoolEntry NewEntry;
	NewEntry.EnergyType = EnergyType;
	NewEntry.Max = InMaxCapacity;
	NewEntry.Current = 0.f;  // Start at zero, accumulate through combat/time
	NewEntry.ChargeRate = ChargeRate;

	EnergyPools.Add(NewEntry);

	UE_LOG(LogFDGAS, Log, TEXT("UFDEnergySet::RegisterEnergyPool - Registered pool: %s, Max: %.1f, ChargeRate: %.2f"),
		*EnergyType.ToString(), InMaxCapacity, ChargeRate);
}

void UFDEnergySet::UnregisterEnergyPool(FGameplayTag EnergyType)
{
	const int32 Index = EnergyPools.IndexOfByPredicate([&EnergyType](const FEnergyPoolEntry& Entry)
	{
		return Entry.EnergyType == EnergyType;
	});

	if (Index != INDEX_NONE)
	{
		UE_LOG(LogFDGAS, Log, TEXT("UFDEnergySet::UnregisterEnergyPool - Removing pool: %s"),
			*EnergyType.ToString());
		EnergyPools.RemoveAt(Index);
	}
}

// ---- Query ----

float UFDEnergySet::GetEnergyCurrent(FGameplayTag EnergyType) const
{
	const FEnergyPoolEntry* Entry = FindPoolEntry(EnergyType);
	return Entry ? Entry->Current : 0.0f;
}

float UFDEnergySet::GetEnergyMax(FGameplayTag EnergyType) const
{
	const FEnergyPoolEntry* Entry = FindPoolEntry(EnergyType);
	return Entry ? Entry->Max : 0.0f;
}

float UFDEnergySet::GetEnergyChargeRate(FGameplayTag EnergyType) const
{
	const FEnergyPoolEntry* Entry = FindPoolEntry(EnergyType);
	return Entry ? Entry->ChargeRate : 0.0f;
}

bool UFDEnergySet::HasEnoughEnergy(FGameplayTag EnergyType, float Amount) const
{
	const FEnergyPoolEntry* Entry = FindPoolEntry(EnergyType);
	if (!Entry)
	{
		return false;
	}
	return Entry->Current >= Amount;
}

float UFDEnergySet::GetEnergyPercent(FGameplayTag EnergyType) const
{
	const FEnergyPoolEntry* Entry = FindPoolEntry(EnergyType);
	if (!Entry || Entry->Max <= 0.0f)
	{
		return 0.0f;
	}
	return FMath::Clamp(Entry->Current / Entry->Max, 0.0f, 1.0f);
}

bool UFDEnergySet::HasEnergyPool(FGameplayTag EnergyType) const
{
	return FindPoolEntry(EnergyType) != nullptr;
}

// ---- Operations ----

void UFDEnergySet::ConsumeEnergy(FGameplayTag EnergyType, float Amount)
{
	FEnergyPoolEntry* Entry = FindPoolEntryMutable(EnergyType);
	if (!Entry)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("UFDEnergySet::ConsumeEnergy - No pool found for Tag: %s"),
			*EnergyType.ToString());
		return;
	}

	const float OldValue = Entry->Current;
	Entry->Current = FMath::Max(0.0f, Entry->Current - Amount);

	UE_LOG(LogFDGAS, Verbose, TEXT("UFDEnergySet::ConsumeEnergy - Tag: %s, Amount: %.1f, %.1f -> %.1f"),
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
		UE_LOG(LogFDGAS, Log, TEXT("UFDEnergySet::ConsumeEnergy - Pool depleted! Tag: %s"),
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

void UFDEnergySet::AddEnergy(FGameplayTag EnergyType, float Amount)
{
	FEnergyPoolEntry* Entry = FindPoolEntryMutable(EnergyType);
	if (!Entry)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("UFDEnergySet::AddEnergy - No pool found for Tag: %s"),
			*EnergyType.ToString());
		return;
	}

	const float OldValue = Entry->Current;
	Entry->Current = FMath::Min(Entry->Max, Entry->Current + Amount);

	UE_LOG(LogFDGAS, Verbose, TEXT("UFDEnergySet::AddEnergy - Tag: %s, Amount: %.1f, %.1f -> %.1f"),
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

// ---- Private Helpers ---- //

const FEnergyPoolEntry* UFDEnergySet::FindPoolEntry(FGameplayTag EnergyType) const
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

FEnergyPoolEntry* UFDEnergySet::FindPoolEntryMutable(FGameplayTag EnergyType)
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

void UFDEnergySet::ClampPool(FEnergyPoolEntry& Entry)
{
	Entry.Current = FMath::Clamp(Entry.Current, 0.0f, Entry.Max);
}
