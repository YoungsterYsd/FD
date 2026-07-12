-- 自动生成，请勿手动修改
-- 来源: 属性表初始化表.xlsx/技能能量表

local SkillEnergy_Config = {
    [1] = {
        RuleID = 2001,
        SubID = 1,
        Direction = "Gain",
        EnergyType = "Energy.Type.Ultimate",
        Amount = 20,
    },
    [2] = {
        RuleID = 2001,
        SubID = 2,
        Direction = "Gain",
        EnergyType = "Energy.Type.Weapon",
        Amount = 30,
    },
    [3] = {
        RuleID = 2002,
        SubID = 1,
        Direction = "Cost",
        EnergyType = "Energy.Type.General",
        Amount = 20,
    },
    [4] = {
        RuleID = 2002,
        SubID = 2,
        Direction = "Gain",
        EnergyType = "Energy.Type.Ultimate",
        Amount = 40,
    },
    [5] = {
        RuleID = 2002,
        SubID = 3,
        Direction = "Gain",
        EnergyType = "Energy.Type.Weapon",
        Amount = 40,
    },
}
return SkillEnergy_Config
