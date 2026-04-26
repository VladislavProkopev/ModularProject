// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "CoreExperienceActionSet.generated.h"

class UGameFeatureAction;

/**
 * Definition of a set of actions to perform as part of entering an experience
 */
UCLASS(BlueprintType,NotBlueprintable)
class UCoreExperienceActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UCoreExperienceActionSet();
	
	//~ UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~ End UObject interface
	
	//~ UPrimaryDataAsset
#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif
	//~ End UPrimaryDataAsset
	
	//List of actions to perform as this experience is loaded/activated/deactivated/unloaded
	UPROPERTY(EditAnywhere,Instanced,Category="Actions to Perform")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;
	
	//List of Game Feature Plugins this experience wants to have active
	UPROPERTY(EditAnywhere,Category="Feature Dependencies")
	TArray<FString> GameFeaturesToEnable;
};
