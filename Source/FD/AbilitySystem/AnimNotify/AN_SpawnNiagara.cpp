// Copyright YoungSterYSD. All Rights Reserved.

#include "AN_SpawnNiagara.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AN_SpawnNiagara)

void UAN_SpawnNiagara::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!NiagaraSystem)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("AN_SpawnNiagara::Notify - NiagaraSystem is null"));
		return;
	}

	if (!MeshComp)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("AN_SpawnNiagara::Notify - MeshComp is null"));
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	UWorld* World = Owner ? Owner->GetWorld() : nullptr;
	if (!World)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("AN_SpawnNiagara::Notify - World is null"));
		return;
	}

	if (bAttached)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			NiagaraSystem,
			MeshComp,
			SocketName,
			LocationOffset,
			RotationOffset,
			EAttachLocation::SnapToTarget,
			true
		);
	}
	else
	{
		const FVector WorldLocation = MeshComp->GetSocketLocation(SocketName);
		const FRotator WorldRotation = MeshComp->GetSocketRotation(SocketName) + RotationOffset;
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			World,
			NiagaraSystem,
			WorldLocation + LocationOffset,
			WorldRotation,
			FVector::OneVector,
			true
		);
	}

	UE_LOG(LogFDGAS, Verbose, TEXT("AN_SpawnNiagara::Notify - System=%s, Socket=%s, Attached=%d"),
		*NiagaraSystem->GetName(), *SocketName.ToString(), bAttached);
}
