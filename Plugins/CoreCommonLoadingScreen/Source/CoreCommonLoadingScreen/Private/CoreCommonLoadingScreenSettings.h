// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "CoreCommonLoadingScreenSettings.generated.h"

class UObject;

/**
 * Settings for load screen system
 */
UCLASS(Config=Game,DefaultConfig,meta = (DisplayName="Common Loading Screen"))
class CORECOMMONLOADINGSCREEN_API UCoreCommonLoadingScreenSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()
public:
	UCoreCommonLoadingScreenSettings();
	
	//The widget to load the loading screen.
	UPROPERTY(Config,EditAnywhere,Category=Display,meta=(MetaClass="/Script/UMG.UserWidget"))
	FSoftClassPath LoadingScreenWidget;
	
	//The z-order of the loading screen widget in the viewport stack
	UPROPERTY(Config,EditAnywhere,Category=Display)
	int32 LoadingScreenZOrder = 10000;
	
	//How long to hold the loading screen up after other loading finishes (in seconds) to
	//try to give texture streaming a chance to avoid blurrines
	//
	//Note: This is not normally applied in the editor for iteration time, but can be
	//enabled via HoldLoadingScreenAdditionalSecsEvenInEditor
	UPROPERTY(Config,EditAnywhere,Category=Configuration,meta=(ForceUnits=s,ConsoleVariable="CommonLoadingScreen.HoldLoadingScreenAdditionsSecs"))
	float HoldLoadingScreenAdditionsSecs = 2.f;
	
	//The interval in seconds beyond witch the loading screen is considered permanently hung(if non-zero)
	UPROPERTY(Config,EditAnywhere,Category=Configuration,meta=(ForceUnits=s))
	float LoadingScreenHeartbeatHangDuration = 0.f;
	
	//The interval in seconds between each log of what is keeping a loading screen up(if non-zero)
	UPROPERTY(Config,EditAnywhere,Category=Configuration,meta=(ForceUnits=s))
	float LogLoadingScreenHeartbeatInterval = 5.f;
	
	//When true, the reason the loading screen is shown or hidden will be printed to the log every frame.
	UPROPERTY(Transient,EditAnywhere,Category=Debugging,meta=(ConsoleVariable="CommonLoadingScreen.LogLoadingScreenReasonEveryFrame"))
	bool LogLoadingScreenReasonEveryFrame = false;
	
	//Force the loading screen to be displayed(useful for debugging)
	UPROPERTY(Transient,EditAnywhere,Category=Debugging,meta=(ConsoleVariable="CommonLoadingScreen.AlwaysShow"))
	bool ForceLoadingScreenVisible = false;
	
	//Shoud we apply additional HoldLoadingScreenAdditionalSecs delay even in the editor
	//(useful when interacting on loading screens)
	UPROPERTY(Transient,EditAnywhere,Category=Debugging)
	bool HoldLoadingScreenAdditionalSecsEvenInEditor = false;
	
	//Should we apply the additional HoldLoadingScreenAdditionalSecs delay even in the editor
	//(useful when interacting on loading screens)
	UPROPERTY(Config,EditAnywhere,Category=Configuration)
	bool ForceTickLoadingScreenEvenInEditor = true;
};
