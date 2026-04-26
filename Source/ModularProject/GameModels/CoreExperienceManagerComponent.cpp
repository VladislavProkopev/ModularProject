// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreExperienceManagerComponent.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "CoreExperienceDefinition.h"
#include "CoreExperienceActionSet.h"
#include "CoreExperienceManager.h"
#include "GameFeaturesSubsystem.h"
#include "ModularProject/System/CoreAssetManager.h"
#include "GameFeatureAction.h"
#include "GameFeaturesSubsystemSettings.h"
#include "TimerManager.h"
//TODO Settings/CoreSettingsLocal
#include "ModularProject/Settings/CoreSettingsLocal.h"
#include "ModularProject/CoreLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CoreExperienceManagerComponent)


//@TODO: Async load the experience definition itself
//@TODO: Handle failures explicitly (go into a 'completed but failed' state rather than check()-ing)
//@TODO: Do the action phases at the appropriate times instead of all at once
//@TODO: Support deactivating an experience and do the unloading actions
//@TODO: Think about what deactivation/cleanup means for preloaded assets
//@TODO: Handle deactivating game features, right now we 'leak' them enabled
// (for a client moving from experience to experience we actually want to diff the requirements and only unload some, not unload everything for them to just be immediately reloaded)
//@TODO: Handle both built-in and URL-based plugins (search for colon?)

namespace CoreConsoleVariables
{
	static float ExperienceLoadRandomDelayMin = 0.f;
	static FAutoConsoleVariableRef CVarExperienceLoadRandomDelayMin(
		TEXT("core.chaos.ExperienceDelayLoad.MinSecs"),
		ExperienceLoadRandomDelayMin,
		TEXT("This value (in seconds) will be added as a delay of load competition of th experience (analog with the random value core.chaos.ExperienceDelayLoad.RandomSecs)"),
		ECVF_Default);
	
	static float ExperienceLoadRandomDelayRange = 0.f;
	static FAutoConsoleVariableRef CVarExperienceLoadRandomDelayRange(
		TEXT("core.chaos.ExperienceDelayLoad.RandomSecs"),
		ExperienceLoadRandomDelayRange,
		TEXT("A random of time between 0 and this value(in seconds) will be added as delay of load completion of the experience (analog with the fixed value core.chaos.ExperienceDelayLoad.MinSecs)"),
		ECVF_Default);
	
	float GetExperienceLoadDelayDuration()
	{
		return FMath::Max(0.f,ExperienceLoadRandomDelayMin + FMath::FRand() * ExperienceLoadRandomDelayRange);
	}
}

// Sets default values for this component's properties
UCoreExperienceManagerComponent::UCoreExperienceManagerComponent(const FObjectInitializer& Initializer) : Super(Initializer)
{
	SetIsReplicatedByDefault(true);
	
}

void UCoreExperienceManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	//Deactivate any features this experience loaded
	//@TODO This shoud be handled FILO as well
	for (const FString& PluginURL : GameFeaturePluginsURLs)
	{
		if (UCoreExperienceManager::RequestToDeactivatePlugin(PluginURL))
		{
			UGameFeaturesSubsystem::Get().DeactivateGameFeaturePlugin(PluginURL);
		}
	}
	
	//@TODO Ensure proper handling of a partialy-loaded state too
	if (LoadState == ECoreExperienceLoadState::Loaded)
	{
		LoadState = ECoreExperienceLoadState::Deactivating;
		
		//Make sure we won't complete the transition permaturely if someone registers as a pauser but fires immediately
		NumExpectedPausers = INDEX_NONE;
		NumObservedPausers = 0;
		
		//Deactivate and unload the actions
		FGameFeatureDeactivatingContext Context(TEXT(""),[this](FStringView){this->OnActionDeactivationCompleted();});
		
		const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
		if (ExistingWorldContext)
		{
			Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
		}
		
		auto DeactivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
		{
			for (UGameFeatureAction* Action : ActionList)
			{
				if (Action)
				{
					Action->OnGameFeatureDeactivating(Context);
					Action->OnGameFeatureUnregistering();
				}
			}
		};
		
		DeactivateListOfActions(CurrentExperience->Actions);
		for (const TObjectPtr<UCoreExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
		{
			if (ActionSet!=nullptr)
			{
				DeactivateListOfActions(ActionSet->Actions);
			}
		}
		
		NumExpectedPausers = Context.GetNumPausers();
		
		if (NumExpectedPausers > 0)
		{
			UE_LOG(LogTemp,Error,TEXT("Actions that have asyncronous deactivation aren't fully supported yet in Core experiences"));
		}

		if (NumExpectedPausers == NumObservedPausers)
		{
			OnAllActionsDeactivated();
		}
	}
}

bool UCoreExperienceManagerComponent::ShouldShowLoadingScreen(FString& OutReason) const
{
	if (LoadState != ECoreExperienceLoadState::Loaded)
	{
		OutReason = TEXT("Experience still loading");
		return true;
	}
	
	return false;
}

void UCoreExperienceManagerComponent::SetCurrentExperience(FPrimaryAssetId ExperienceId)
{
	UCoreAssetManager& AssetManager = UCoreAssetManager::Get();
	FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(ExperienceId);
	TSubclassOf<UCoreExperienceDefinition> AssetClass = Cast<UClass>(AssetPath.TryLoad());
	check(AssetClass);
	const UCoreExperienceDefinition* Exprerience = GetDefault<UCoreExperienceDefinition>(AssetClass);
	
	check(Exprerience!=nullptr);
	check(CurrentExperience == nullptr);
	CurrentExperience = Exprerience;
	StartExperienceLoad();
}

void UCoreExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_HightPriority(
	FOnCoreExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		OnCoreExperienceLoaded_HighPriority.Add(MoveTemp(Delegate));
	}
}

void UCoreExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnCoreExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		OnCoreExperienceLoaded.Add(MoveTemp(Delegate));
	}
}

void UCoreExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_LowPriority(
	FOnCoreExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		OnCoreExperienceLoaded_LowPriority.Add(MoveTemp(Delegate));
	}
}

const UCoreExperienceDefinition* UCoreExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	check(LoadState == ECoreExperienceLoadState::Loaded);
	check(CurrentExperience != nullptr);
	return CurrentExperience;
}

bool UCoreExperienceManagerComponent::IsExperienceLoaded() const
{
	return (LoadState == ECoreExperienceLoadState::Loaded) && (CurrentExperience != nullptr);
}

void UCoreExperienceManagerComponent::OnRep_CurrentExperience()
{
	StartExperienceLoad();
}

void UCoreExperienceManagerComponent::StartExperienceLoad()
{
	check(CurrentExperience != nullptr);
	check(LoadState == ECoreExperienceLoadState::UnLoaded);
	
	UE_LOG(LogTemp,Log,TEXT("EXPERIENCE: StartExperienceLoad(CurrentExperience = %s, %s)"),
		*CurrentExperience->GetPrimaryAssetId().ToString(),
		//TODO CoreLogChannels
		*GetClientServerContextString(this));
	
	LoadState = ECoreExperienceLoadState::Loading;
	
	UCoreAssetManager& AssetManager = UCoreAssetManager::Get();
	
	TSet<FPrimaryAssetId> BundleAssetList;
	TSet<FSoftObjectPath> RawAssetList;
	
	BundleAssetList.Add(AssetManager.GetPrimaryAssetId());
	for (const TObjectPtr<UCoreExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if (ActionSet != nullptr)
		{
			BundleAssetList.Add(ActionSet->GetPrimaryAssetId());
		}
	}
	
	//Load assets associated with the experience
	TArray<FName> BundleToLoad;
	
	//@TODO Centralize this client/server stuff into the CoreAssetManager
	const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
	const bool bLoadClient = GIsEditor || (OwnerNetMode != NM_DedicatedServer);
	const bool bLoadServer = GIsEditor || (OwnerNetMode != NM_Client);
	if (bLoadClient)
	{
		BundleToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
	}
	if (bLoadServer)
	{
		BundleToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);
	}
	
	TSharedPtr<FStreamableHandle> BundleLoadHandle = nullptr;
	if (BundleAssetList.Num() > 0)
	{
		BundleLoadHandle = AssetManager.ChangeBundleStateForPrimaryAssets(BundleAssetList.Array(), BundleToLoad,{},false,FStreamableDelegate(),FStreamableManager::AsyncLoadHighPriority);
	}
	
	TSharedPtr<FStreamableHandle> RawLoadHandle = nullptr;
	if (RawAssetList.Num() > 0)
	{
		BundleLoadHandle = AssetManager.LoadAssetList(RawAssetList.Array(),FStreamableDelegate(),FStreamableManager::AsyncLoadHighPriority,TEXT("StartExperienceLoad()"));
	}
	
	//If both async loads are running, combine item
	TSharedPtr<FStreamableHandle> Handle = nullptr;
	if (BundleLoadHandle.IsValid() && RawLoadHandle.IsValid())
	{
		Handle = AssetManager.GetStreamableManager().CreateCombinedHandle({BundleLoadHandle,RawLoadHandle});
	}
	else
	{
		Handle = BundleLoadHandle.IsValid() ? BundleLoadHandle : RawLoadHandle;
	}
	
	FStreamableDelegate OnAssetsLoadedDelegate = FStreamableDelegate::CreateUObject(this,&ThisClass::OnExperienceLoadComplete);
	if (!Handle.IsValid() || Handle->HasLoadCompleted())
	{
		//Assets were already loaded, call the delegate now
		FStreamableHandle::ExecuteDelegate(OnAssetsLoadedDelegate);
	}
	else
	{
		Handle->BindCompleteDelegate(OnAssetsLoadedDelegate);
		
		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetsLoadedDelegate]()
		{
			OnAssetsLoadedDelegate.ExecuteIfBound();
		}));
	}
	
	//This set of assets gets preloaded, but we don't block the start of the experience based on it
	TSet<FPrimaryAssetId> PreloadAssetList;
	//@TODO Determine assets to preload (but not blocking-ly)
	if (PreloadAssetList.Num() > 0)
	{
		AssetManager.ChangeBundleStateForPrimaryAssets(PreloadAssetList.Array(),BundleToLoad,{});
	}
}

void UCoreExperienceManagerComponent::OnExperienceLoadComplete()
{
	check(LoadState == ECoreExperienceLoadState::Loading);
	check(CurrentExperience != nullptr);
	
	UE_LOG(LogTemp,Log,TEXT("EXPERIENCE: OnExperienceLoadComplete(CurrentExperience = %s, %s)"),
		*CurrentExperience->GetPrimaryAssetId().ToString(),
		*GetClientServerContextString(this));
	
	// Find the URL's our GameFeaturesPlugins - filtering out dupes and ones that don't have a valid mapping
	GameFeaturePluginsURLs.Reset();
	
	auto CollectGameFeaturePluginURLs = [This=this](const UPrimaryDataAsset* Context, const TArray<FString>& FeaturePluginList)
	{
		for (const FString& PluginName : FeaturePluginList)
		{
			FString PluginURL;
			if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName,/*Out*/ PluginURL))
			{
				This->GameFeaturePluginsURLs.AddUnique(PluginURL);
			}
			else
			{
				ensureMsgf(false,TEXT("OnExperienceLoadComplete failed to find plugin URL from PluginName %s for Experience %s - fix data, ignoring for this run"),*PluginName,*Context->GetPrimaryAssetId().ToString());
			}
		}
		
		// 		// Add in our extra plugin
		// 		if (!CurrentPlaylistData->GameFeaturePluginToActivateUntilDownloadedContentIsPresent.IsEmpty())
		// 		{
		// 			FString PluginURL;
		// 			if (UGameFeaturesSubsystem::Get().GetPluginURLByName(CurrentPlaylistData->GameFeaturePluginToActivateUntilDownloadedContentIsPresent, PluginURL))
		// 			{
		// 				GameFeaturePluginURLs.AddUnique(PluginURL);
		// 			}
		// 		}
	};
	
	CollectGameFeaturePluginURLs(CurrentExperience,CurrentExperience->GameFeaturesToEnable);
	for (const TObjectPtr<UCoreExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if (ActionSet != nullptr)
		{
			CollectGameFeaturePluginURLs(ActionSet,ActionSet->GameFeaturesToEnable);
		}
	}
	
	//Load and activate the features
	NumGameFeaturePluginsLoading = GameFeaturePluginsURLs.Num();
	if (NumGameFeaturePluginsLoading > 0)
	{
		LoadState = ECoreExperienceLoadState::LoadingGameFeatures;
		for (const FString& PluginURL : GameFeaturePluginsURLs)
		{
			UCoreExperienceManager::NotifyOfPluginActivation(PluginURL);
			UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL,FGameFeaturePluginLoadComplete::CreateUObject(this,&ThisClass::OnGameFeaturePluginLoadComplete));
			
		}
	}
	else
	{
		OnExperienceFullLoadComplete();
	}
}

void UCoreExperienceManagerComponent::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)
{
	//Decrement the number of plugins that are loading
	NumGameFeaturePluginsLoading--;

	if (NumGameFeaturePluginsLoading == 0)
	{
		OnExperienceFullLoadComplete();
	}
}

void UCoreExperienceManagerComponent::OnExperienceFullLoadComplete()
{
	check(LoadState != ECoreExperienceLoadState::Loaded);
	
	//Insert a random delay for testing (if configured)
	if (LoadState != ECoreExperienceLoadState::LoadingChaosTesingDelay)
	{
		const float DelaySecs = CoreConsoleVariables::GetExperienceLoadDelayDuration();
		if (DelaySecs > 0.f)
		{
			FTimerHandle DummyHandle;
			
			LoadState = ECoreExperienceLoadState::LoadingChaosTesingDelay;
			GetWorld()->GetTimerManager().SetTimer(DummyHandle,this,&ThisClass::OnExperienceFullLoadComplete,DelaySecs,false);
			
			return;
		}
	}
	
	LoadState = ECoreExperienceLoadState::ExecutingActions;
	
	//Execute the actions
	FGameFeatureActivatingContext Context;
	
	//Only apply to our specific world context if set
	const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
	if (ExistingWorldContext)
	{
		Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
	}
	
	auto ActivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
	{
		for (UGameFeatureAction* Action : ActionList)
		{
			if (Action!=nullptr)
			{
				//@TODO the fact that these don't take a world are potentialy problematic in client-server PIE
				//The current behaviour matches systems like gameplay tags where loading and registering apply to the entire process,
				//but actually applying the results to actors is restricted to specific world
				Action->OnGameFeatureRegistering();
				Action->OnGameFeatureLoading();
				Action->OnGameFeatureActivating(Context);
			}
		}
	};
	
	ActivateListOfActions(CurrentExperience->Actions);
	for (const TObjectPtr<UCoreExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if (ActionSet != nullptr)
		{
			ActivateListOfActions(ActionSet->Actions);
		}
	}
	
	LoadState = ECoreExperienceLoadState::Loaded;
	
	OnCoreExperienceLoaded_HighPriority.Broadcast(CurrentExperience);
	OnCoreExperienceLoaded_HighPriority.Clear();
	
	OnCoreExperienceLoaded.Broadcast(CurrentExperience);
	OnCoreExperienceLoaded.Clear();
	
	OnCoreExperienceLoaded_LowPriority.Broadcast(CurrentExperience);
	OnCoreExperienceLoaded_LowPriority.Clear();
	
	//Apply any necessary scalability settings
#if !UE_SERVER
	//TODO CoreSettingsLocal
	//UCoreSettingsLocal::Get()->OnExperienceLoaded();
#endif
	
}

void UCoreExperienceManagerComponent::OnActionDeactivationCompleted()
{
	check(IsInGameThread());
	++NumObservedPausers;

	if (NumObservedPausers == NumExpectedPausers)
	{
		OnAllActionsDeactivated();
	}
}

void UCoreExperienceManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutProps) const
{
	Super::GetLifetimeReplicatedProps(OutProps);
	
	//DOREPLIFETIME(ThisClass, CurrentExperience);
}

void UCoreExperienceManagerComponent::OnAllActionsDeactivated()
{
	//@TODO We actually only deactivated and didn't fully unload...
	LoadState = ECoreExperienceLoadState::UnLoaded;
	CurrentExperience = nullptr;
	//TODO GEngine->ForceGarbageCollection(true);
}


