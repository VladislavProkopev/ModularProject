// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CoreInputConfig.h"
#include "ProjectPawnData.generated.h"

class UGameplayEffect;
class UCoreGameplayAbility;
class UCoreAttributeSet;
class UCoreAbilityTagRelationshipMapping;

#define PROJ_API MODULARPROJECT_API
/**
 * 
 */
UCLASS(MinimalAPI,BlueprintType,Const,Meta = (DisplayName = "Pawn Data", ShortTooltip = "Data asset used to define a Pawn"))
class UProjectPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	PROJ_API UProjectPawnData(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category= "Project|Pawn")
	TSubclassOf<APawn> PawnClass;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category= "Project|Input")
	TObjectPtr<UCoreInputConfig> InputConfig;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category= "Project|Abilities")
	TObjectPtr<UCoreAbilityTagRelationshipMapping> TagRelationshipMapping;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category= "Project|Abilities")
	TArray<TObjectPtr<UCoreAttributeSet>> AbilitySets;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category= "Project|Abilities")
	TObjectPtr<UCoreGameplayAbility> StartupAbilityTEST;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category= "Project|GAS")
	TSubclassOf<UGameplayEffect> DefaultAttributesGE;
};

#undef PROJ_API