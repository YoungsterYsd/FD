// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "FDInputConfig.generated.h"

class UInputAction;

/**
 * 单个 InputAction → GameplayTag 的映射项。
 */
USTRUCT(BlueprintType)
struct FFDTaggedInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> InputAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * 输入配置 —— 定义 InputAction 到 GameplayTag 的映射。
 * 由 HeroComponent 读取并绑定到 Pawn 的 InputComponent。
 */
UCLASS(BlueprintType, Const)
class FD_API UFDInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	/** 由 HeroComponent 直接处理的输入（移动/交互/菜单等） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TArray<FFDTaggedInputAction> NativeInputActions;

	/** 转发到 ASC 的技能输入（QWER） */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TArray<FFDTaggedInputAction> AbilityInputActions;
};
