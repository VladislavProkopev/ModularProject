// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "CoreAttributeSet.generated.h"

class AActor;
class UCoreAbilitySystemComponent;
class UObject;
class UWorld;
struct FGameplayEffectSpec;

#define GAS_API COREGAS_API

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS(MinimalAPI)
class UCoreAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	GAS_API UCoreAttributeSet();
	
	GAS_API UWorld* GetWorld() const override;
	
	GAS_API UCoreAbilitySystemComponent* GetCoreAbilitySystemComponent() const;
};

#undef GAS_API