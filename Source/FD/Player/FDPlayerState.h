// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "FDPlayerState.generated.h"

class UAbilitySystemComponent;

/**
 * FD 项目 PlayerState —— 玩家 ASC 的宿主。
 * 实现 IAbilitySystemInterface 供外部获取 ASC。
 */
UCLASS()
class FD_API AFDPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AFDPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};
