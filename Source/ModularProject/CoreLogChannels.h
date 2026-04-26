#pragma once

#include "Logging/LogMacros.h"

class UObject;

MODULARPROJECT_API DECLARE_LOG_CATEGORY_EXTERN(LogProjectCore,Log,All);
MODULARPROJECT_API DECLARE_LOG_CATEGORY_EXTERN(LogCoreExperience,Log,All);
MODULARPROJECT_API DECLARE_LOG_CATEGORY_EXTERN(LogCoreAbilitySystem,Log,All);
MODULARPROJECT_API DECLARE_LOG_CATEGORY_EXTERN(LogCoreTeams,Log,All);

MODULARPROJECT_API FString GetClientServerContextString(UObject* ContextObject = nullptr);

