// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayEffectTypes.h"
#include "UFDAnimInstance.generated.h"

class UBlendSpace;
class UAbilitySystemComponent;

UCLASS()
class FD_API UFDAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	/** ASC 就绪时由 FDAbilitySystemComponent 调用，建立 Tag→变量 自动映射 */
	void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

	/** 移动混合空间 — ABP Class Defaults 中配置 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UBlendSpace> LocomotionBlendSpace;

	/** 水平移动速度 (cm/s) */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	float Speed = 0.0f;

	/** 移动方向 [-180°, 180°] */
	UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
	float Direction = 0.0f;

	/** 是否死亡 */
	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsDead = false;

protected:
	void RefreshLocomotion();

	/** Tag→变量 自动映射表（ABP Class Defaults 或 C++ 中配置映射关系） */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;
};
