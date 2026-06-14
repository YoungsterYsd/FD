// Copyright YoungSterYSD. All Rights Reserved.

#include "FDPlayerState.h"
#include "AbilitySystem/FDAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDPlayerState)

AFDPlayerState::AFDPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UFDAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

UAbilitySystemComponent* AFDPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
