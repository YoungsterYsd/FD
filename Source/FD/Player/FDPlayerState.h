// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "FDPlayerState.generated.h"

class UFDAbilitySystemComponent;

/**
 * FD 项目 PlayerState —— 玩家 ASC 的宿主。
 * 实现 IAbilitySystemInterface 供外部获取 ASC。
 * 使用自定义 UFDAbilitySystemComponent 支持 Enhanced Input 技能激活管线。
 */
UCLASS()
class FD_API AFDPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AFDPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Typed accessor for FD ASC. */
	UFUNCTION(BlueprintCallable, Category = "FD|PlayerState")
	UFDAbilitySystemComponent* GetFDAbilitySystemComponent() const { return AbilitySystemComponent; }

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UFDAbilitySystemComponent> AbilitySystemComponent;
};
