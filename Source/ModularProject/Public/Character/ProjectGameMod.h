// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectGameMod.generated.h"

struct FStreamableHandle;
class UProjectPawnData;

#define PROJ_API MODULARPROJECT_API
/**
 * 
 */
UCLASS(MinimalAPI)
class AProjectGameMod : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	TSharedPtr<FStreamableHandle> PawnDataLoadHandle;
	
	PROJ_API void PreloadPawnData(TSoftObjectPtr<UProjectPawnData> SoftPawnData);
	
	UFUNCTION()
	PROJ_API void OnPawnDataLoaded();
	
};

#undef PROJ_API
