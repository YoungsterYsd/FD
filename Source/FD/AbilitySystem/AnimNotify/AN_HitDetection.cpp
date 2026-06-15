// Copyright YoungSterYSD. All Rights Reserved.

#include "AN_HitDetection.h"
#include "AbilitySystem/Abilities/FDGameplayAbility.h"
#include "AbilitySystem/Configs/FDSkillInfoRow.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "LogChannels/FDLogChannels.h"
#include "CollisionShape.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AN_HitDetection)

void UAN_HitDetection::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("AN_HitDetection::Notify - MeshComp is null"));
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("AN_HitDetection::Notify - Owner is null"));
		return;
	}

	UWorld* World = Owner->GetWorld();
	if (!World)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("AN_HitDetection::Notify - World is null"));
		return;
	}

	UAbilitySystemComponent* OwnerASC = GetOwnerASC(MeshComp);
	if (!OwnerASC)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("AN_HitDetection::Notify - No ASC on owner"));
		return;
	}

	// Find the active FDGameplayAbility to read skill data
	UFDGameplayAbility* ActiveGA = nullptr;
	for (const FGameplayAbilitySpec& Spec : OwnerASC->GetActivatableAbilities())
	{
		if (Spec.IsActive() && Spec.Ability)
		{
			ActiveGA = Cast<UFDGameplayAbility>(Spec.Ability);
			if (ActiveGA)
			{
				break;
			}
		}
	}

	if (!ActiveGA)
	{
		UE_LOG(LogFDGAS, Warning, TEXT("AN_HitDetection::Notify - No active UFDGameplayAbility found"));
		return;
	}

	const int32 SkillID = ActiveGA->SkillID;
	const int32 SkillLevel = ActiveGA->SkillLevel;
	const int32 CurrentSeg = ActiveGA->CurrentSeg;

	const FFDSkillInfoRow* SkillInfo = FFDSkillInfoRow::Find(SkillID, SkillLevel, CurrentSeg);
	float AtkMul = 1.0f;
	float BasicVal = 0.0f;
	if (SkillInfo)
	{
		AtkMul = SkillInfo->AtkMul;
		BasicVal = SkillInfo->BasicVal;
	}

	// Calculate damage (simplified; C4 will use full formula)
	const float Damage = BasicVal * AtkMul;

	// Build collision shape and query
	const FVector OwnerLocation = Owner->GetActorLocation();
	const FQuat OwnerRotation = Owner->GetActorRotation().Quaternion();
	const ECollisionChannel TraceChannel = ECC_Pawn;

	FCollisionShape CollisionShape;
	TArray<FHitResult> OutHits;

	switch (ShapeType)
	{
	case EFDHitShapeType::Sphere:
		CollisionShape = FCollisionShape::MakeSphere(Radius);
		World->SweepMultiByChannel(OutHits, OwnerLocation, OwnerLocation, OwnerRotation,
			TraceChannel, CollisionShape);
		break;

	case EFDHitShapeType::Capsule:
		CollisionShape = FCollisionShape::MakeCapsule(Radius, HalfHeight);
		World->SweepMultiByChannel(OutHits, OwnerLocation, OwnerLocation, OwnerRotation,
			TraceChannel, CollisionShape);
		break;

	case EFDHitShapeType::Box:
		CollisionShape = FCollisionShape::MakeBox(BoxExtent);
		World->SweepMultiByChannel(OutHits, OwnerLocation, OwnerLocation, OwnerRotation,
			TraceChannel, CollisionShape);
		break;

	case EFDHitShapeType::Sector:
	{
		// Sector: sphere sweep + angle filtering
		CollisionShape = FCollisionShape::MakeSphere(Radius);
		World->SweepMultiByChannel(OutHits, OwnerLocation, OwnerLocation, OwnerRotation,
			TraceChannel, CollisionShape);

		const FVector Forward = Owner->GetActorForwardVector();
		const float HalfAngleCos = FMath::Cos(FMath::DegreesToRadians(SectorAngle * 0.5f));

		for (int32 i = OutHits.Num() - 1; i >= 0; --i)
		{
			const FVector ToTarget = (OutHits[i].GetActor()->GetActorLocation() - OwnerLocation).GetSafeNormal();
			if (FVector::DotProduct(Forward, ToTarget) < HalfAngleCos)
			{
				OutHits.RemoveAt(i);
			}
		}
		break;
	}
	}

	UE_LOG(LogFDGAS, Verbose, TEXT("AN_HitDetection::Notify - Shape=%d, Radius=%.0f, Hits=%d, Damage=%.0f"),
		static_cast<int32>(ShapeType), Radius, OutHits.Num(), Damage);

	// Apply damage to each valid target
	int32 HitCount = 0;
	for (const FHitResult& Hit : OutHits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor)
		{
			continue;
		}

		// Skip self
		if (bIgnoreSelf && HitActor == Owner)
		{
			continue;
		}

		UAbilitySystemComponent* TargetASC = GetOwnerASC(
			HitActor->FindComponentByClass<USkeletalMeshComponent>());
		if (!TargetASC)
		{
			TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor);
		}

		if (!TargetASC)
		{
			continue;
		}

		// Target tag filtering
		if (!TargetRequiredTags.IsEmpty() && !TargetASC->HasAllMatchingGameplayTags(TargetRequiredTags))
		{
			continue;
		}

		// Apply damage GE
		if (DamageGE)
		{
			FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(DamageGE, 1.0f,
				OwnerASC->MakeEffectContext());
			if (SpecHandle.IsValid())
			{
				SpecHandle.Data->SetSetByCallerMagnitude(
					FGameplayTag::RequestGameplayTag(TEXT("SetByCaller.Damage")), Damage);
				OwnerASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, TargetASC);
				++HitCount;
			}
		}
	}

	UE_LOG(LogFDGAS, Verbose, TEXT("AN_HitDetection::Notify - Applied damage to %d targets"), HitCount);
}

UAbilitySystemComponent* UAN_HitDetection::GetOwnerASC(USkeletalMeshComponent* MeshComp)
{
	if (!MeshComp)
	{
		return nullptr;
	}
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner());
}
