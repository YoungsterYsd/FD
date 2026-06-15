// Copyright YoungSterYSD. All Rights Reserved.

#include "AN_PlaySound.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AN_PlaySound)

void UAN_PlaySound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!Sound)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("AN_PlaySound::Notify - Sound is null"));
		return;
	}

	if (!MeshComp)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("AN_PlaySound::Notify - MeshComp is null"));
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	UWorld* World = Owner ? Owner->GetWorld() : nullptr;
	if (!World)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("AN_PlaySound::Notify - World is null"));
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(World, Sound, Owner->GetActorLocation(),
		VolumeMultiplier, PitchMultiplier);

	UE_LOG(LogFDGAS, Verbose, TEXT("AN_PlaySound::Notify - Sound=%s, Vol=%.2f, Pitch=%.2f"),
		*Sound->GetName(), VolumeMultiplier, PitchMultiplier);
}
