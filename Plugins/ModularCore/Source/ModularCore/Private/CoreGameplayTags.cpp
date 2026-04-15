#include "CoreGameplayTags.h"

namespace CoreGameplayTags
{
	namespace NativeInput
	{
		UE_DEFINE_GAMEPLAY_TAG(Input_Move, "NativeInput.Move");
		UE_DEFINE_GAMEPLAY_TAG(Input_Look, "NativeInput.Look");
	}

	namespace Abilities
	{
		UE_DEFINE_GAMEPLAY_TAG(Ability_TEST, "Abilities.TEST");
		UE_DEFINE_GAMEPLAY_TAG(Ability_AbilityInputBlocked, "Abilities.AbilityInputBlocked");
		UE_DEFINE_GAMEPLAY_TAG(Ability_Behavior_SurvivesDeath, "Abilities.Behaviour.SurvivesDeath");
	}

	namespace GameFeaturesDataStates
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_Spawned,"InitState.Spawned", "1: Actor/component has initially spawned and can be extented");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_DataAvaliable,"InitState.DataAvaliable","2: All required data has been loaded/replicated and is ready for initialization");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_DataInitialized,"InitState.DataInitialized","3: The avaliable data has been initialized for this actor/component, but it is not ready for fuul gameplay");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_GameplayReady,"InitState.GameplayReady","4: The actor/component is fully ready for active gameplay");
	}
}
