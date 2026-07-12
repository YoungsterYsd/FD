-- [[ 配置清单 —— 纯数据，无逻辑
--     新增配置类型 = 在此数组加一个 {} 块
--     版本对比：diff 此文件即可
--     StructPath（UScriptStruct 路径）是配置类型的唯一标识
-- ]]

local ConfigManifest = {
    -- ===== 属性类配置 =====
   	[1] = {
		Module = "Config.Hero_Attr_Init",
		StructPath = "/Script/FD.FDHeroInitData",
	},
	[2] = {
		Module = "Config.Monster_Attr_Init",
		StructPath = "/Script/FD.FDMonsterInitData",
	},
	[3] = {
		Module = "Config.Energy_Config",
		StructPath = "/Script/FD.FDEnergyConfigData",
	},
	[4] = {
		Module = "Config.DamageBonus_Config",
		StructPath = "/Script/FD.FDDamageBonusConfigData",
	},

    -- ===== 技能类配置 =====
    [5] = {
        Module = "Config.SkillInfo_Config",
        StructPath = "/Script/FD.FDSkillInfoData",
    },
    [6] = {
        Module = "Config.SkillParams_Config",
        StructPath = "/Script/FD.FDSkillParamData",
    },
    [7] = {
        Module = "Config.SkillEnergy_Config",
        StructPath = "/Script/FD.FDSkillEnergyData",
    },

    -- ===== 任务类配置（未来）=====
    -- { Module = "Config.Quest_Data", StructPath = "/Script/FD.FFDQuestData" },
}

return ConfigManifest
