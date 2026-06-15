// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AN_HitDetection.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;

/**
 * Hit detection shape types for AN_HitDetection.
 */
UENUM(BlueprintType)
enum class EFDHitShapeType : uint8
{
	Sphere   UMETA(DisplayName = "Sphere"),
	Capsule  UMETA(DisplayName = "Capsule"),
	Box      UMETA(DisplayName = "Box"),
	Sector   UMETA(DisplayName = "Sector")
};

/**
 * AnimNotify that performs a shape-based sweep on the owning character to detect hit targets
 * and apply a damage GameplayEffect via SetByCaller.Damage.
 *
 * Reads SkillID / SkillLevel / CurrentSeg from the currently active UFDGameplayAbility
 * and fetches AtkMul / BasicVal from DT_SkillInfo to compute damage.
 *
 * @see UFDGameplayAbility, FFDSkillInfoRow
 */
UCLASS(Blueprintable, EditInlineNew, meta = (DisplayName = "Hit Detection"))
class FD_API UAN_HitDetection : public UAnimNotify
{
	GENERATED_BODY()

public:
	/** Shape used for the sweep. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitDetection")
	EFDHitShapeType ShapeType = EFDHitShapeType::Sphere;

	/** Radius for Sphere / Capsule / Sector shapes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitDetection",
		meta = (EditCondition = "ShapeType!=EFDHitShapeType::Box"))
	float Radius = 100.0f;

	/** Half-height for Capsule / Box shapes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitDetection",
		meta = (EditCondition = "ShapeType==EFDHitShapeType::Capsule||ShapeType==EFDHitShapeType::Box"))
	float HalfHeight = 50.0f;

	/** Extent (X/Y/Z half-lengths) for Box shape. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitDetection",
		meta = (EditCondition = "ShapeType==EFDHitShapeType::Box"))
	FVector BoxExtent = FVector(50.0f, 50.0f, 50.0f);

	/** Angle in degrees for Sector sweep. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitDetection",
		meta = (EditCondition = "ShapeType==EFDHitShapeType::Sector"))
	float SectorAngle = 60.0f;

	/** Targets must have ALL of these tags to receive damage. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitDetection")
	FGameplayTagContainer TargetRequiredTags;

	/** Damage GameplayEffect class to apply (Instant, with SetByCaller.Damage modifier). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitDetection")
	TSubclassOf<UGameplayEffect> DamageGE;

	/** If true, skip the owning actor in the hit results. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitDetection")
	bool bIgnoreSelf = true;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	/** Get the AbilitySystemComponent from a MeshComponent's owning actor. */
	static UAbilitySystemComponent* GetOwnerASC(USkeletalMeshComponent* MeshComp);
};
