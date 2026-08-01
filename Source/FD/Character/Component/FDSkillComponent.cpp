// Copyright YoungSterYSD. All Rights Reserved.

#include "FDSkillComponent.h"
#include "Character/Data/FDAbilitySet.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDSkillComponent)

UFDSkillComponent::UFDSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFDSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	if (DefaultAbilitySet)
	{
		GrantAbilitySet(DefaultAbilitySet);
	}
}

void UFDSkillComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Find ASC for cleanup
	AActor* Owner = GetOwner();
	UAbilitySystemComponent* ASC = nullptr;
	if (Owner)
	{
		ASC = Owner->FindComponentByClass<UAbilitySystemComponent>();
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
	}

	// Revoke all active sets
	for (auto& Kvp : GrantedHandlesMap)
	{
		if (Kvp.Key && ASC)
		{
			Kvp.Key->RemoveFromAbilitySystem(ASC, &Kvp.Value);
		}
	}
	GrantedHandlesMap.Empty();

	Super::EndPlay(EndPlayReason);
}

void UFDSkillComponent::GrantAbilitySet(UFDAbilitySet* Set)
{
	if (!Set)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// Find ASC on owner first, then on PlayerState
	UAbilitySystemComponent* ASC = Owner->FindComponentByClass<UAbilitySystemComponent>();
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

	if (!ASC)
	{
		return;
	}

	// Check if already granted
	if (GrantedHandlesMap.Contains(Set))
	{
		return;
	}

	FFDAbilitySet_GrantedHandles Handles;
	Set->GiveToAbilitySystem(ASC, &Handles, Owner);
	GrantedHandlesMap.Add(Set, MoveTemp(Handles));
}

void UFDSkillComponent::RevokeAbilitySet(UFDAbilitySet* Set)
{
	if (!Set)
	{
		return;
	}

	FFDAbilitySet_GrantedHandles* Handles = GrantedHandlesMap.Find(Set);
	if (!Handles)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	UAbilitySystemComponent* ASC = Owner->FindComponentByClass<UAbilitySystemComponent>();
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
		Set->RemoveFromAbilitySystem(ASC, Handles);
	}

	GrantedHandlesMap.Remove(Set);
}
