// Copyright YoungSterYSD. All Rights Reserved.

#include "ANS_InputBuffer.h"
#include "AbilitySystem/FDAbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "LogChannels/FDLogChannels.h"
#include "GameFramework/Actor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ANS_InputBuffer)

// Static member definition
TMap<TWeakObjectPtr<AActor>, UANS_InputBuffer*> UANS_InputBuffer::ActiveBuffers;

void UANS_InputBuffer::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("ANS_InputBuffer::NotifyBegin - MeshComp is null"));
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("ANS_InputBuffer::NotifyBegin - Owner is null"));
		return;
	}

	// Register this buffer for the owning actor
	ActiveBuffers.Add(Owner, this);
	BufferedInputs.Reset();

	CachedASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner);

	UE_LOG(LogFDGAS, Verbose, TEXT("ANS_InputBuffer::NotifyBegin - Registered for %s, Duration=%.2f"),
		*Owner->GetName(), TotalDuration);
}

void UANS_InputBuffer::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	// No per-tick processing needed; inputs are buffered via BufferInput() as they arrive
}

void UANS_InputBuffer::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	// Unregister from active buffers
	ActiveBuffers.Remove(Owner);

	UAbilitySystemComponent* ASC = CachedASC.Get();
	if (!ASC)
	{
		ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner);
	}

	if (!ASC)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("ANS_InputBuffer::NotifyEnd - No ASC on owner"));
		BufferedInputs.Reset();
		return;
	}

	// Process buffered inputs: map InputTag → AbilityTag and try to activate
	for (const FGameplayTag& InputTag : BufferedInputs)
	{
		const FGameplayTag* AbilityTag = InputTagToAbilityTag.Find(InputTag);
		if (AbilityTag && AbilityTag->IsValid())
		{
			UE_LOG(LogFDGAS, Verbose,
				TEXT("ANS_InputBuffer::NotifyEnd - Routing buffered input %s → ability %s"),
				*InputTag.ToString(), *AbilityTag->ToString());

			// Trigger ability activation via the same input press path
			if (UFDAbilitySystemComponent* FDASC = Cast<UFDAbilitySystemComponent>(ASC))
			{
				FDASC->AbilityInputTagPressed(*AbilityTag);
			}
		}
	}

	UE_LOG(LogFDGAS, Verbose, TEXT("ANS_InputBuffer::NotifyEnd - Processed %d buffered inputs"),
		BufferedInputs.Num());

	BufferedInputs.Reset();
	CachedASC.Reset();
}

void UANS_InputBuffer::BufferInput(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	BufferedInputs.AddUnique(InputTag);

	UE_LOG(LogFDGAS, Verbose, TEXT("ANS_InputBuffer::BufferInput - Buffered %s (total=%d)"),
		*InputTag.ToString(), BufferedInputs.Num());
}
