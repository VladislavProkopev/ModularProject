// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectGameMod.generated.h"

class UProjectPawnData;
/**
 * 
 */
UCLASS()
class MODULARPROJECT_API AProjectGameMod : public AGameModeBase
{
	GENERATED_BODY()
	
private:
	void PreloadPawnData(TSoftObjectPtr<UProjectPawnData> SoftPawnData);
};
