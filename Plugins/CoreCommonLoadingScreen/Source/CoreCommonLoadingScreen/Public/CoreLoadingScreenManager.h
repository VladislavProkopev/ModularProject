// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "UObject/WeakInterfacePtr.h"
#include "CoreLoadingScreenManager.generated.h"

#define UE_API CORECOMMONLOADINGSCREEN_API

template<typename InterfaceType> class TScriptInterface;

class FSubsystemCollectionBase;
class IInputProcessor;
class ILoadingProcessInterface;
class SWidget;
class UObject;
class UWorld;
struct FFrame;
struct FWorldContext;

/**
 * Handles showing/hiding the loading screen
 */
UCLASS(MinimalAPI)
class UCoreLoadingScreenManager : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	//~ USubsystem interface
	UE_API virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	UE_API virtual void Deinitialize() override;
	UE_API virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~ End USubsystem interface
	
	//~ FTickableObjectBase Interface
	UE_API virtual void Tick(float DeltaTime) override;
	UE_API virtual ETickableTickType GetTickableTickType() const override;
	UE_API virtual bool IsTickable() const override;
	UE_API TStatId GetStatId() const override;
	UE_API UWorld* GetTickableGameObjectWorld() const override;
	//~ End FTickableObjectBase Interface
	
	UFUNCTION(BlueprintCallable, Category = LoadingScreen)
	FString GetDebugReasunForShowingOrHideLoadingScreen() const
	{
		return DebugReasonForShowingOrHidingLoadingScreen;
	}
	
	/** Returns True when the loading screen is currently being shown */
	bool GetLoadingScreenDisplayStatus() const
	{
		return bCurrentlyShowingLoadingScreen;
	}
	
	/** Called when the loading screen visibility changes */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoadingScreenVisibilityChangedDelegate, bool);
	FORCEINLINE FOnLoadingScreenVisibilityChangedDelegate& GetOnLoadingScreenVisibilityChangedDelegate(){return LoadingScreenVisibilityChanged;}
	
	UE_API void RegisterLoadingProcessor(TScriptInterface<ILoadingProcessInterface> Interface);
	UE_API void UnregisterLoadingProcessor(TScriptInterface<ILoadingProcessInterface> Interface);
	
private:
	UE_API void HandlePreloadMap(const FWorldContext& WorldContext, const FString& MapName);
	UE_API void HandlePostLoadMap(UWorld* World);
	
	/** Determines if we should show of hide the loading screen. Called every frame*/
	UE_API void UpdateLoadingScreen();
	
	/** Returns true if we need to be showing the loading screen. */
	UE_API bool CheckForAnyNeedToShowLoadingScreen();
	
	/** Returns true if we want to be showing the loading screen (if we need to or are artificially forcing it on for other reasons). */
	UE_API bool ShouldShowLoadingScreen();
	
	/** Returns true if we are in initial loading flow before this screen should be used */
	UE_API bool IsShowingInitialLoadingScreen() const;
	
	/** Shows the loading screen. Sets up the loading screen widget on the viewport */
	UE_API void ShowLoadingScreen();
	
	/** Hides the loading screen. The loading screen widget will be destroyed. */
	UE_API void HideLoadingScreen();
	
	/** Removes the widget from viewport */
	UE_API void RemoveWidgetFromViewport();
	
	/** Prevents input from being used in-game while the loading screen is visible */
	UE_API void StartBlockingInput();
	
	/** Resumes in-game input, if blocked */
	UE_API void StopBlockingInput();
	
	UE_API void ChangePerformSettings(bool bEnabingLoadingScreen);
	
private:
	/** Delegate broadcast when loading screen visibility changes */
	FOnLoadingScreenVisibilityChangedDelegate LoadingScreenVisibilityChanged;
	
	/** A reference to the loading screen widget we are displaying (if any) */
	TSharedPtr<SWidget> LoadingScreenWidget;
	
	/** Input processor to eat all input while loading screen is shown */
	TSharedPtr<IInputProcessor> InputPreProcessor;
	
	/** External loading processors, components maybe actors that delay the loading */
	TArray<TWeakInterfacePtr<ILoadingProcessInterface>> ExternalLoadingProcessors;
	
	/** The reason why loading screen is up (or not) */
	FString DebugReasonForShowingOrHidingLoadingScreen;
	
	/** The time when we started showing the loading screen */
	double TimeLoadingScreenShown = 0.0;
	
	/** The time the loading screen most recently wanted to be dismissed (might still be up due to a min display duration requrement) */
	double TimeLoadingScreenLastDismissed = -1.0;
	
	/** The time untill the next log for why loading screen is still up */
	double TimeUntilNextLogHeartbeatSeconds = 0.0;
	
	/** True when we are between PreLoadMap and PostLoadMap */
	bool bCurrentlyInLoadMap = false;
	
	/** True when the loading screen is currently being shown */
	bool bCurrentlyShowingLoadingScreen = false;
};

#undef UE_API
