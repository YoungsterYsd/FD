// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

/**
 * Macro to generate Get/Set/Init accessor functions for a gameplay attribute.
 * Expands to:
 *   static FGameplayAttribute Get##PropertyName##Attribute();
 *   float Get##PropertyName##() const;
 *   void Set##PropertyName##(float NewVal);
 *   void Init##PropertyName##(float NewVal);
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * Multicast delegate for attribute change events.
 * Modeled after Lyra's FLyraAttributeEvent.
 * Parameters: EffectInstigator, EffectCauser, EffectSpec, EffectMagnitude, OldValue, NewValue.
 */
DECLARE_MULTICAST_DELEGATE_SixParams(FFDAttributeEvent,
	AActor* /*EffectInstigator*/,
	AActor* /*EffectCauser*/,
	const struct FGameplayEffectSpec* /*EffectSpec*/,
	float /*EffectMagnitude*/,
	float /*OldValue*/,
	float /*NewValue*/);
