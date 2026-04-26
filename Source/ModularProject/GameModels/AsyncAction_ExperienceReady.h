// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_ExperienceReady.generated.h"

class AGameStateBase;
class UCoreExperienceDefinition;
class UWorld;
struct FFrame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExperienceReadyAsyncDelegate);

/**
 * Asynctonously wait for the game state to be ready and valid and then calls the OneReady event. Will call OnReady
 * immediately if the game state is valid already
 */
UCLASS()
class UAsyncAction_ExperienceReady : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()
	
public:
	//Waits for the experience to be determined and loaded
	UFUNCTION(BlueprintCallable,meta=(WorldContext = "WorldContextObject",BlueprintInternalUseOnly="true"))
	static UAsyncAction_ExperienceReady* WaitForExperienceReady(const UObject* WorldContextObject);
	
	virtual void Activate() override;
	
	//Called when the experience has been determined and is ready/loaded
	UPROPERTY(BlueprintAssignable)
	FExperienceReadyAsyncDelegate OnReady;
	
private:
	void Step1_HandleGameStateSet(AGameStateBase* GameState);
	void Step2_ListenToExperienceLoading(AGameStateBase* GameState);
	void Step3_HandleExperienceLoaded(const UCoreExperienceDefinition* CurrentExperience);
	void Step4_BroadcastReady();
	
	TWeakObjectPtr<UWorld> WorldPtr;
};
