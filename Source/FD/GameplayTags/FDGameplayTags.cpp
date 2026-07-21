// Copyright YoungSterYSD. All Rights Reserved.

#include "FDGameplayTags.h"

namespace FDGameplayTags
{
	// 角色属性
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Attack,         "Attribute.Attack",         "攻击力");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Defense,        "Attribute.Defense",        "防御力");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Health,         "Attribute.Health",         "生命值");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_HealthMax,      "Attribute.HealthMax",      "最大生命值");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Healing,        "Attribute.Healing",        "治疗效果");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_CritRate,       "Attribute.CritRate",       "暴击率");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_CritDamage,     "Attribute.CritDamage",     "暴击伤害");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_AttackSpeed,    "Attribute.AttackSpeed",    "攻击速度");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_CastSpeed,      "Attribute.CastSpeed",      "施法速度");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_DamageIncrease, "Attribute.DamageIncrease", "伤害增加");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_DamageDecrease, "Attribute.DamageDecrease", "伤害减免");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_BodyStrength,   "Attribute.BodyStrength",   "体格强度");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Tenacity,       "Attribute.Tenacity",       "韧性");

	// 输入
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Move,     "InputTag.Move",     "移动");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Look,     "InputTag.Look",     "视角");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Jump,     "InputTag.Jump",     "跳跃");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Interact, "InputTag.Interact", "交互");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability1, "InputTag.Ability.1","技能1(Q)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability2, "InputTag.Ability.2","技能2(W)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability3, "InputTag.Ability.3","技能3(E)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Ability4, "InputTag.Ability.4","技能4(R)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Camera_Zoom, "InputTag.Camera.Zoom", "相机缩放");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_ClickMove, "InputTag.ClickMove", "点击寻路");

	// 初始化状态
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_Spawned,          "InitState.Spawned",          "已生成");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_DataAvailable,    "InitState.DataAvailable",    "数据就绪");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_DataInitialized,  "InitState.DataInitialized",  "数据已初始化");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_GameplayReady,    "InitState.GameplayReady",    "可开始游戏");

	// 角色状态
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death_Dying, "Status.Death.Dying", "濒死中");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Death_Dead,  "Status.Death.Dead",  "已死亡");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Dodging,     "Status.Dodging",     "闪避无敌帧");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Parrying,    "Status.Parrying",    "招架窗口");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Recovery,        "Status.Recovery",        "后摇恢复");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_MovementLocked,     "Status.MovementLocked",     "技能动画中禁止移动");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Animation_Playing,  "Status.Animation.Playing",  "技能Montage播放中");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Status_Animation_UpperBody,"Status.Animation.UpperBody","上半身混合模式");

	// 游戏事件
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_Death, "GameplayEvent.Death", "死亡事件");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_Reset, "GameplayEvent.Reset", "重置事件");

	// 动态数值传入
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Damage, "SetByCaller.Damage", "动态伤害值");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Heal,   "SetByCaller.Heal",   "动态治疗值");

	// 消息通道
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_Experience_Loaded, "Message.Experience.Loaded", "Experience加载完成");

	// 移动模式
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_Grounded, "FD.Movement.Mode.Grounded", "地面");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Movement_Mode_InAir,    "FD.Movement.Mode.InAir",    "空中");
}
