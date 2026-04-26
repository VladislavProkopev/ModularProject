// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "CoreExperienceDefinition.generated.h"

class UGameFeatureAction;
class UProjectPawnData;
class UCoreExperienceActionSet;

/**
 * Definition of an experience
 */
UCLASS(BlueprintType,Const)
class MODULARPROJECT_API UCoreExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UCoreExperienceDefinition();
	
	//~ UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~ End UObject interface
	
	//~ UPrimaryDataAsset interface
#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif
	//~ End UPrimaryDataAsset interface
	
	//List of Game Feature Plugins this experience wants to have active
	TArray<FString> GameFeaturesToEnable;
	
	// The default pawn class spawn for players
	UPROPERTY(EditDefaultsOnly,Category=Gameplay)
	TSoftObjectPtr<UProjectPawnData> DefaultPawnData;
	
	//List of actions to perform as this experience is loaded/activated/deactivated/unpoaded
	UPROPERTY(EditDefaultsOnly,Category=Actions,Instanced)
	TArray<TObjectPtr<UGameFeatureAction>> Actions;
	
	//List of additional actions sets to compare into this experience
	UPROPERTY(EditDefaultsOnly,Category=Gameplay)
	TArray<TObjectPtr<UCoreExperienceActionSet>> ActionSets;
	
};
