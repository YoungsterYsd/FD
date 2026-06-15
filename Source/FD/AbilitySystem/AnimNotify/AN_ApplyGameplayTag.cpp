// Copyright YoungSterYSD. All Rights Reserved.

#include "AN_ApplyGameplayTag.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AN_ApplyGameplayTag)

void UAN_ApplyGameplayTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("AN_ApplyGameplayTag::Notify - MeshComp is null"));
		return;
	}

	if (!Tag.IsValid())
	{
		UE_LOG(LogFDGAS, Warning, TEXT("AN_ApplyGameplayTag::Notify - Tag is invalid"));
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(
		MeshComp->GetOwner());
	if (!ASC)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("AN_ApplyGameplayTag::Notify - No ASC on owner"));
		return;
	}

	if (bAdd)
	{
		ASC->AddLooseGameplayTag(Tag);
		UE_LOG(LogFDGAS, Verbose, TEXT("AN_ApplyGameplayTag::Notify - Added tag %s"), *Tag.ToString());
	}
	else
	{
		ASC->RemoveLooseGameplayTag(Tag);
		UE_LOG(LogFDGAS, Verbose, TEXT("AN_ApplyGameplayTag::Notify - Removed tag %s"), *Tag.ToString());
	}
}
