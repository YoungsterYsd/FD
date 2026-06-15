// Copyright YoungSterYSD. All Rights Reserved.

#include "ANS_IFrameWindow.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ANS_IFrameWindow)

void UANS_IFrameWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp || !IFrameTag.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(
		MeshComp->GetOwner());
	if (ASC)
	{
		ASC->AddLooseGameplayTag(IFrameTag);
		UE_LOG(LogFDGAS, Verbose, TEXT("ANS_IFrameWindow::NotifyBegin - Added i-frame tag %s, Duration=%.2f"),
			*IFrameTag.ToString(), TotalDuration);
	}
}

void UANS_IFrameWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp || !IFrameTag.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(
		MeshComp->GetOwner());
	if (ASC)
	{
		ASC->RemoveLooseGameplayTag(IFrameTag);
		UE_LOG(LogFDGAS, Verbose, TEXT("ANS_IFrameWindow::NotifyEnd - Removed i-frame tag %s"),
			*IFrameTag.ToString());
	}
}
