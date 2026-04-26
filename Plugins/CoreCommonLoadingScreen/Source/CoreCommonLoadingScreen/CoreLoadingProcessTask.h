// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LoadingProcessInterface.h"
#include "UObject/Object.h"
#include "CoreLoadingProcessTask.generated.h"

#define UE_API CORECOMMONLOADINGSCREEN_API

struct FFrame;

/**
 * 
 */
UCLASS(MinimalAPI,BlueprintType)
class UCoreLoadingProcessTask : public UObject , public ILoadingProcessInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable,meta=(WorldContext="WorldContextObject"))
	static UE_API UCoreLoadingProcessTask* CreateCoreLoadingProcessTask(UObject* WorldContextObject,const FString& ShowLoadingScreenReason);
	
	UCoreLoadingProcessTask(){}
	
	UFUNCTION(BlueprintCallable)
	UE_API void Unregister();
	
	UFUNCTION(BlueprintCallable)
	UE_API void SetShowLoadingScreenReason(const FString& InReason);
	
	UE_API virtual bool ShouldShowLoadingScreen(FString& OutReason);
	
	FString Reason;
};

#undef UE_API