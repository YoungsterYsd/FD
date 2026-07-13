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

	// 获取当前正在播放的 Montage
	UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
	if (!AnimInst)
	{
		return;
	}

	UAnimMontage* PlayingMontage = AnimInst->GetCurrentActiveMontage();
	if (!PlayingMontage)
	{
		return;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(
		MeshComp->GetOwner());
	if (!ASC)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("AN_EndAbility::Notify - No ASC on owner"));
		return;
	}

	// 只取消 AbilityAnimation.Montage 匹配当前播放 Montage 的 GA
	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (!Spec.IsActive() || !Spec.Ability)
		{
			continue;
		}

		UFDGameplayAbility* GA = Cast<UFDGameplayAbility>(Spec.Ability);
		if (GA && GA->AbilityAnimation.Montage == PlayingMontage)
		{
			UE_LOG(LogFDGAS, Verbose, TEXT("AN_EndAbility - Ending GA SkillID=%d via Montage=%s"),
				GA->SkillID, *PlayingMontage->GetName());
			ASC->CancelAbilityHandle(Spec.Handle);
			return;
		}
	}
}
