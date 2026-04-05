// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "ProjectPlayerState.generated.h"

class UCoreAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class MODULARPROJECT_API AProjectPlayerState : public APlayerState , public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AProjectPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	//~ Begin IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface
	
protected:
	UPROPERTY(VisibleAnywhere,Category="Core|PlayerState")
	TObjectPtr<UCoreAbilitySystemComponent> AbilitySystemComponent;
};
