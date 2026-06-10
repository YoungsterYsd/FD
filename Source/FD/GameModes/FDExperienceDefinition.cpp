// Copyright YoungSterYSD. All Rights Reserved.

#include "FDExperienceDefinition.h"
#include "FDExperienceActionSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FDExperienceDefinition)

UFDExperienceDefinition::UFDExperienceDefinition()
{
}

#if WITH_EDITOR
EDataValidationResult UFDExperienceDefinition::IsDataValid(FDataValidationContext& Context) const
{
    EDataValidationResult Result = Super::IsDataValid(Context);

    // 后续阶段校验 PawnData / Actions 不为空

    return Result;
}
#endif
