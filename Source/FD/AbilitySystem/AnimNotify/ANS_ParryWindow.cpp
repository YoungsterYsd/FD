// Copyright YoungSterYSD. All Rights Reserved.

#include "ANS_ParryWindow.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ANS_ParryWindow)

void UANS_ParryWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp || !ParryTag.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(
		MeshComp->GetOwner());
	if (ASC)
	{
		ASC->AddLooseGameplayTag(ParryTag);
		UE_LOG(LogFDGAS, Verbose, TEXT("ANS_ParryWindow::NotifyBegin - Added parry tag %s, Duration=%.2f"),
			*ParryTag.ToString(), TotalDuration);
	}
}

void UANS_ParryWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp || !ParryTag.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(
		MeshComp->GetOwner());
	if (ASC)
	{
		ASC->RemoveLooseGameplayTag(ParryTag);
		UE_LOG(LogFDGAS, Verbose, TEXT("ANS_ParryWindow::NotifyEnd - Removed parry tag %s"),
			*ParryTag.ToString());
	}
}
