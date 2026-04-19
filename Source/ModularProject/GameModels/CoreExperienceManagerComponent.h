/*// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/GameStateComponent.h"
//TODO CoreLoadingProcessInrerface
#include "CoreLoadingProcessInrerface.h"
#include "CoreExperienceManagerComponent.generated.h"

#define MOD_API MODULARPROJECT_API

namespace UE::GameFeatures{	struct FResult;}

//TODO UCoreExperienceDefinition
class UCoreExperienceDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCoreExperienceLoaded,const UCoreExperienceDefinition* /*Experience #1#);

enum class ECoreExperienceLoadState
{
	UnLoaded,
	Loading,
	LoadingGameFeatures,
	LoadingChaosTesingDelay,
	ExecutingActions,
	Loaded,
	Deactivating
};

UCLASS(MinimalAPI)
class UCoreExperienceManagerComponent : public UGameStateComponent , public ILoadingProcessInrerface
{
	GENERATED_BODY()

public:
	MOD_API UCoreExperienceManagerComponent(const FObjectInitializer& Initializer = FObjectInitializer::Get());
	
	//~ UActorComponent Interface
	MOD_API virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UActorComponent Interface
	
	//~ ILoadingProcessInterface
	MOD_API virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	//~ End ILoadingProcessInterface
	
	//Tries to set the current experience, either a UI or gameplay one
	MOD_API void SetCurrentExperience(FPrimaryAssetId ExperienceId);
	
	//Ensures the delegate is called once the experience has beel loaded,
	//before others are called.
	//However, if the experience has already loaded, calls delegate immediately.
	MOD_API void CallOrRegister_OnExperienceLoaded_HightPriority(FOnCoreExperienceLoaded::FDelegate&& Delegate);
	
	//Ensures the delegate is called once the experience has loaded
	//If the experience has already loaded, calls the delegare immediately
	MOD_API void CallOrRegister_OnExperienceLoaded(FOnCoreExperienceLoaded::FDelegate&& Delegate);

	//Ensures the delegate is called once the experience has loaded
	//If the experience has already loaded, calls the delegare immediately
	MOD_API void CallOrRegister_OnExperienceLoaded_LowPriority(FOnCoreExperienceLoaded::FDelegate&& Delegate);
	
	//This returns the current experience if it fully loaded, asserting therwise
	//(i.e., if you called it too soon)
	MOD_API const UCoreExperienceDefinition* GetCurrentExperienceChecked() const;
	
	//Returns true if the experience is fully loaded
	MOD_API bool IsExperienceLoaded() const;
	
private:
	UFUNCTION()
	void OnRep_CurrentExperience();
	
	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadComplete();
	
	void OnActivationDeactivationCompleted();
	void OnAllActionsDeactivated();
	
	
	UPROPERTY(ReplicatedUsing=OnRep_CurrentExperience)
	TObjectPtr<const UCoreExperienceDefinition> CurrentExperience;
	
	ECoreExperienceLoadState LoadState = ECoreExperienceLoadState::UnLoaded;
	
	int32 NumGameFeaturePluginsLoading = 0;
	TArray<FString> GameFeaturePluginsURLs;
	
	int32 NumObservedPausers = 0;
	int32 NumExpectedPausers = 0;
	
	/**
	 * Delegate called when the experience has finished loading juist before others
	 * (e.g., subsystems that set up for regular gameplay)
	 #1#
	FOnCoreExperienceLoaded OnCoreExperienceLoaded_HighPriority;
	
	/** Delegate called when the experience has finished loading #1#
	FOnCoreExperienceLoaded OnCoreExperienceLoaded;
	
	/** Delegate called when the experience has finished loading #1#
	FOnCoreExperienceLoaded OnCoreExperienceLoaded_LowPriority;
};
#undef MOD_API*/