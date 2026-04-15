#pragma once
#include "NativeGameplayTags.h"

namespace CoreGameplayTags
{
	namespace NativeInput
	{
		MODULARCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Move);
		MODULARCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Look);
	}

	namespace Abilities
	{
		MODULARCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_TEST);
		MODULARCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_AbilityInputBlocked);
		MODULARCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Behavior_SurvivesDeath);
	}

	namespace GameFeaturesDataStates
	{
		MODULARCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
		MODULARCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvaliable);
		MODULARCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
		MODULARCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);
	}
}
