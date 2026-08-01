// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "Components/PawnComponent.h"
#include "GameplayTagContainer.h"
#include "FDGameHeroComponent.generated.h"

class UFDInputConfig;
class UInputComponent;
struct FInputActionValue;

/**
 * 输入桥接组件 —— 读取 UFDInputConfig，绑定 IA 到回调。
 * 技能输入预留向 ASC 转发的扩展点。
 */
UCLASS()
class FD_API UFDGameHeroComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UFDGameHeroComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 绑定输入 —— 由 AFDCharacter::SetupPlayerInputComponent 调用 */
	void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	/** 设置输入配置 —— 由 PawnData 驱动 */
	void SetInputConfig(const UFDInputConfig* InConfig) { InputConfig = InConfig; }

protected:
	/** WASD 移动回调 */
	void Input_Move(const FInputActionValue& Value);

	/** 跳跃按下 */
	void Input_Jump(const FInputActionValue& Value);

	/** 跳跃释放 */
	void Input_StopJump(const FInputActionValue& Value);

	/** 技能按键按下 —— 后续阶段经缓冲层转发到 ASC */
	void Input_AbilityPressed(FGameplayTag InputTag);

	/** 技能按键释放 */
	void Input_AbilityReleased(FGameplayTag InputTag);

	/** 交互键 */
	void Input_Interact(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<const UFDInputConfig> InputConfig;

	// ===== 战斗状态 =====

public:
	/** Whether the character is currently locked in battle with a target. */
	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsInBattle() const { return bInBattle; }

	/** Enter or exit battle mode. Disables orient-to-movement when in battle. */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetInBattle(bool bEnabled);

	/** Set the enemy actor to face during battle. */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetBattleTarget(AActor* Target);

	// ===== 战斗状态 =====

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bInBattle = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<AActor> BattleTarget;
};
