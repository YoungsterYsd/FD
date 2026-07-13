// Copyright YoungSterYSD. All Rights Reserved.

#include "UFDAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "LogChannels/FDLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UFDAnimInstance)

void UFDAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 自动从 Owner 查找 ASC 并初始化 PropertyMap
	if (const AActor* Owner = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner))
		{
			InitializeWithAbilitySystem(ASC);
		}
	}
}

void UFDAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	if (!ASC) return;
	GameplayTagPropertyMap.Initialize(this, ASC);
	UE_LOG(LogFD, Log, TEXT("[FDAnim] AnimInstance PropertyMap initialized with ASC"));
}

void UFDAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	RefreshLocomotion();
}

void UFDAnimInstance::RefreshLocomotion()
{
	const ACharacter* Character = Cast<ACharacter>(TryGetPawnOwner());
	if (!Character)
	{
		return;
	}

	const UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
	if (!Movement)
	{
		return;
	}

	const FVector Velocity = Movement->Velocity;
	Speed = Velocity.Size2D();

	if (Speed > KINDA_SMALL_NUMBER)
	{
		const FRotator ActorRotation = Character->GetActorRotation();
		const FVector VelocityDir = Velocity.GetSafeNormal2D();
		Direction = FMath::RadiansToDegrees(
			FMath::Atan2(
				FVector::DotProduct(ActorRotation.RotateVector(FVector::RightVector), VelocityDir),
				FVector::DotProduct(ActorRotation.RotateVector(FVector::ForwardVector), VelocityDir)));
	}
	else
	{
		Direction = 0.0f;
	}
}


