// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FDAbilitySystemComponent.generated.h"

/**
 * FD 项目自定义 AbilitySystemComponent。
 *
 * 扩展 UAbilitySystemComponent，添加 Enhanced Input 驱动的技能激活管线：
 * - AbilityInputTagPressed: 输入按下时，查找匹配 InputTag 的技能并缓存 Handle。
 * - AbilityInputTagReleased: 输入释放时，标记技能结束。
 * - ProcessAbilityInput: 每帧由 PlayerController Tick 调用，处理按下/按住/释放队列。
 *
 * 玩家 ASC 挂在 AFDPlayerState 上，怪物 ASC 直接挂在 Character 上。
 */
UCLASS()
class FD_API UFDAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	/**
	 * 处理技能输入按下。
	 * 遍历所有 ActivatableAbilities，找到 DynamicAbilityTags 中包含 InputTag 的技能，
	 * 将其 SpecHandle 加入 Pressed 和 Held 队列。
	 * @param InputTag - 输入 GameplayTag（如 InputTag.Ability1）。
	 */
	void AbilityInputTagPressed(const FGameplayTag& InputTag);

	/**
	 * 处理技能输入释放。
	 * 将匹配 InputTag 的技能 SpecHandle 从 Held 移入 Released 队列。
	 * @param InputTag - 输入 GameplayTag。
	 */
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	/**
	 * 每帧处理技能输入队列。
	 * - Pressed: 尝试激活技能（TryActivateAbility）。
	 * - Held: 持续按住中的技能（可用于蓄力等）。
	 * - Released: 调用 AbilitySpecInputReleased 通知技能输入结束。
	 * @param DeltaTime - 帧时间。
	 * @param bGamePaused - 游戏是否暂停。
	 */
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

private:
	/** 当前帧按下的技能 Handle 列表。 */
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	/** 当前帧按住中的技能 Handle 列表。 */
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	/** 当前帧释放的技能 Handle 列表。 */
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
};
