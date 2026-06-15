// Copyright YoungSterYSD. All Rights Reserved.

#include "AN_EndAbility.h"
#include "AbilitySystem/Abilities/FDGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AN_EndAbility)

void UAN_EndAbility::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("AN_EndAbility::Notify - MeshComp is null"));
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(
		MeshComp->GetOwner());
	if (!ASC)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("AN_EndAbility::Notify - No ASC on owner"));
		return;
	}

	// Find the active UFDGameplayAbility and cancel it
	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.IsActive() && Spec.Ability)
		{
			UFDGameplayAbility* GA = Cast<UFDGameplayAbility>(Spec.Ability);
			if (GA)
			{
				UE_LOG(LogFDGAS, Verbose, TEXT("AN_EndAbility::Notify - Cancelling GA SkillID=%d"),
					GA->SkillID);
				ASC->CancelAbilityHandle(Spec.Handle);
			}
		}
	}
}
