// Copyright YoungSterYSD. All Rights Reserved.

#include "ANS_RecoveryWindow.h"
#include "AbilitySystem/Abilities/FDGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTags/FDGameplayTags.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ANS_RecoveryWindow)

void UANS_RecoveryWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp)
	{
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(
		MeshComp->GetOwner());
	if (ASC)
	{
		ASC->AddLooseGameplayTag(FDGameplayTags::Status_Recovery);
		UE_LOG(LogFDGAS, Verbose, TEXT("ANS_RecoveryWindow::NotifyBegin - Recovery started, Duration=%.2f"),
			TotalDuration);
	}
}

void UANS_RecoveryWindow::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp || CancelInputTags.Num() == 0)
	{
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(
		MeshComp->GetOwner());
	if (!ASC)
	{
		return;
	}

	// Check if any of the cancel input tags have been pressed
	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (!Spec.IsActive() || !Spec.Ability)
		{
			continue;
		}

		UFDGameplayAbility* GA = Cast<UFDGameplayAbility>(Spec.Ability);
		if (!GA)
		{
			continue;
		}

		// Check if any cancel input tag matches the ability's dynamic tags
		const FGameplayTagContainer& AbilityTags = Spec.GetDynamicSpecSourceTags();
		for (const FGameplayTag& CancelTag : CancelInputTags)
		{
			if (AbilityTags.HasTag(CancelTag))
			{
				UE_LOG(LogFDGAS, Verbose,
					TEXT("ANS_RecoveryWindow::NotifyTick - Cancelling ability SkillID=%d via tag %s"),
					GA->SkillID, *CancelTag.ToString());
				ASC->CancelAbilityHandle(Spec.Handle);
				return;
			}
		}
	}
}

void UANS_RecoveryWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(
		MeshComp->GetOwner());
	if (ASC)
	{
		ASC->RemoveLooseGameplayTag(FDGameplayTags::Status_Recovery);
		UE_LOG(LogFDGAS, Verbose, TEXT("ANS_RecoveryWindow::NotifyEnd - Recovery ended"));
	}
}
