// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "CoreGameData.generated.h"

#define MOD_API MODULARPROJECT_API

struct FGameplayTag;
class UGameplayEffect;
class UObject;

/**
 * UCoreGameData
 * 
 * Non-Mutable data asset that contains global game data
 */

UCLASS(MinimalAPI,BlueprintType,Const, Meta = (DisplayName = "Core GameData",ShortTooltip = "Data asset containing global game data."))
class UCoreGameData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	MOD_API UCoreGameData();
	
	//Returns the loaded game data
	static MOD_API const UCoreGameData& Get();
	
	//Gameplay effect used to apply damage. Uses SetByCaller for the damage magnitude.
	UPROPERTY(EditDefaultsOnly,Category="Default Gameplay Effects", Meta = (DisplayName = "Damage Gameplay Effect (SetByCaller)"))
	TSoftClassPtr<UGameplayEffect> DamageGameplayEffect_SetByCaller;
	
	//Gameplay effect used to apply healing. Uses SetByCaller for the healing magnitude.
	UPROPERTY(EditDefaultsOnly,Category="Default Gameplay Effects", Meta = (DisplayName = "Heal Gameplay Effects (SetByCaller)"))
	TSoftClassPtr<UGameplayEffect> HealGameplayEffect_SetByCaller;
	
	//Gameplay effect used to add and remove dynamic tags.
	UPROPERTY(EditDefaultsOnly,Category="Default Gameplay Effects")
	TSoftClassPtr<UGameplayEffect> DynamicTagGameplayEffect;
	
	UPROPERTY(EditDefaultsOnly,Category="Gameplay Cues", Meta = (Categories = "Gameplay Cues"))
	TArray<FGameplayTag> AlwaysLoadedGameplayCues;
};

#undef MOD_API