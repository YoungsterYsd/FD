// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_SpawnNiagara.generated.h"

class UNiagaraSystem;

/**
 * AnimNotify that spawns a Niagara particle effect at a socket or world location.
 */
UCLASS(Blueprintable, EditInlineNew, meta = (DisplayName = "Spawn Niagara"))
class FD_API UAN_SpawnNiagara : public UAnimNotify
{
	GENERATED_BODY()

public:
	/** Niagara system to spawn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	TObjectPtr<UNiagaraSystem> NiagaraSystem;

	/** Socket or bone name to attach from. Use NAME_None for root. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	FName SocketName = NAME_None;

	/** Rotation offset applied to the spawned effect. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	FRotator RotationOffset = FRotator::ZeroRotator;

	/** Location offset relative to the socket. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	FVector LocationOffset = FVector::ZeroVector;

	/** If true, attach the effect to the socket; otherwise spawn at world location. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	bool bAttached = true;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};
