// Copyright YoungSterYSD. All Rights Reserved.

#pragma once

#include "FDEnergyConfigRow.h"
#include "FDDamageBonusConfigRow.h"
#include "FDCharacterInitRow.h"
#include "FDMonsterInitRow.h"
#include "FDSkillInfoRow.h"
#include "FDSkillParamRow.h"
#include "FDSkillEnergyRow.h"

// Call once at module startup to register all table parsers
inline void FDTableMgr_RegisterAllParsers()
{
	FFDEnergyConfigRow::Register();
	FFDDamageBonusConfigRow::Register();
	FFDCharacterInitRow::Register();
	FFDMonsterInitRow::Register();
	FFDSkillInfoRow::Register();
	FFDSkillParamRow::Register();
	FFDSkillEnergyRow::Register();
}
