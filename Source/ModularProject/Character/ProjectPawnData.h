// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CoreInputConfig.h"
#include "ProjectPawnData.generated.h"

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
};

#undef PROJ_API