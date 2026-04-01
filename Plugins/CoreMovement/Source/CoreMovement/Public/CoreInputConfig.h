// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "CoreInputConfig.generated.h"

class UInputAction;
class UObject;
struct FFrame;

USTRUCT(BlueprintType)
struct FCoreInputAction
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
	
};

/**
 * 
 */
UCLASS(BlueprintType,Const)
class COREMOVEMENT_API UCoreInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UCoreInputConfig(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable,Category="Core|Pawn")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag InputTag, bool bLogNotFound = true) const;
	
	UFUNCTION(BlueprintCallable,Category="Core|Pawn")
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag InputTag, bool bLogNotFound = true) const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FCoreInputAction> NativeInputActions;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FCoreInputAction> AbilityInputActions;
};
