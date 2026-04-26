// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreExperienceManager.h"
#include "ModularProject/GameModels/CoreExperienceManager.h"
#include "Engine/Engine.h"
#include "Subsystems/SubsystemCollection.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CoreExperienceManager)


#if WITH_EDITOR
void UCoreExperienceManager::OnPlayInEditorBegun()
{
	ensure(GameFeaturePluginRequestCountMap.IsEmpty());
	GameFeaturePluginRequestCountMap.Empty();
}

void UCoreExperienceManager::NotifyOfPluginActivation(const FString PluginURL)
{
	if (GIsEditor)
	{
		UCoreExperienceManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<UCoreExperienceManager>();
		check(ExperienceManagerSubsystem);
		
		//Track the number of requesters who activate this plugin. Multiple load/activation requests are always allowed because concurrent requests are handled.
		int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindOrAdd(PluginURL);
		++Count;
	}
}

bool UCoreExperienceManager::RequestToDeactivatePlugin(const FString PluginURL)
{
	if (GIsEditor)
	{
		UCoreExperienceManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<UCoreExperienceManager>();
		check(ExperienceManagerSubsystem);
		
		//Only let the last requester to get this far deactivate the plugin
		int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindChecked(PluginURL);
		--Count;

		if (Count == 0)
		{
			ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.Remove(PluginURL);
			return true;
		}
		
		return false;
	}
	
	return true;
}
#endif