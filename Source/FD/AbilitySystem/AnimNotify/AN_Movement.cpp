// Copyright YoungSterYSD. All Rights Reserved.

#include "AN_Movement.h"
#include "GameFramework/Actor.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AN_Movement)

void UAN_Movement::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("AN_Movement::Notify - MeshComp is null"));
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("AN_Movement::Notify - Owner is null"));
		return;
	}

	const FVector CurrentLocation = Owner->GetActorLocation();
	const FVector ActorForward = Owner->GetActorForwardVector();
	const FVector ActorRight = Owner->GetActorRightVector();
	const FVector ActorUp = Owner->GetActorUpVector();

	const FVector WorldDirection =
		ActorForward * DirectionOffset.X +
		ActorRight * DirectionOffset.Y +
		ActorUp * DirectionOffset.Z;

	const FVector NewLocation = CurrentLocation + WorldDirection.GetSafeNormal() * Distance;

	UE_LOG(LogFDGAS, Verbose, TEXT("AN_Movement::Notify - Owner=%s, Dist=%.0f, Process=%d, From=%s, To=%s"),
		*Owner->GetName(), Distance, bProcessMovement, *CurrentLocation.ToString(), *NewLocation.ToString());

	if (!bProcessMovement)
	{
		// Instant teleport
		Owner->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
	}
	else
	{
		// Process-based movement: reserved for future C5 AbilityTask integration
		UE_LOG(LogFDGAS, Verbose,
			TEXT("AN_Movement::Notify - bProcessMovement=true, interpolation deferred to C5 AbilityTask"));
	}
}
