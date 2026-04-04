// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ActiveGameplayEffectHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "Templates/SubclassOf.h"
#include "GameplayAbilitySpecHandle.h"
#include "UObject/ObjectKey.h"
#include "CoreGlobalAbilitySystem.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UCoreAbilitySystemComponent;
class UObject;
struct FActiveGameplayEffectHandle;
struct FFrame;
struct FGameplayAbilitySpecHandle;

USTRUCT()
struct FGlobalAppliedAbilityList
{
	GENERATED_BODY()
	
	TMap<FObjectKey,FGameplayAbilitySpecHandle> Handles;
	
	void AddToASC(TSubclassOf<UGameplayAbility> Ability,UCoreAbilitySystemComponent* ASC);
	void RemoveFromASC(UCoreAbilitySystemComponent* ASC);
	void RemoveFromAll();
	
};

USTRUCT()
struct FGlobalAppliedEffectList
{
	GENERATED_BODY()
	
	TMap<FObjectKey,FActiveGameplayEffectHandle> Handles;
	
	void AddToASC(TSubclassOf<UGameplayEffect> Effect,UCoreAbilitySystemComponent* ASC);
	void RemoveFromASC(UCoreAbilitySystemComponent* ASC);
	void RemoveFromAll();
};
/**
 * 
 */
UCLASS()
class COREGAS_API UCoreGlobalAbilitySystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable,BlueprintAuthorityOnly,Category=CoreGAS)
	void ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability);
	
	UFUNCTION(BlueprintCallable,BlueprintAuthorityOnly,Category=CoreGAS)
	void ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect);
	
	UFUNCTION(BlueprintCallable,BlueprintAuthorityOnly,Category=CoreGAS)
	void RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability);
	
	UFUNCTION(BlueprintCallable,BlueprintAuthorityOnly,Category=CoreGAS)
	void RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect);
	
	/** Register an ASC with global system and apply any active global effects/abilities. */
	void RegisterASC(UCoreAbilitySystemComponent* ASC);
	
	/** Removes an ASC from the global system, along with any active global effects/abilities. */
	void UnregisterASC(UCoreAbilitySystemComponent* ASC);
	
private:
	UPROPERTY()
	TMap<TSubclassOf<UGameplayAbility>,FGlobalAppliedAbilityList> AppliedAbilities;
	
	UPROPERTY()
	TMap<TSubclassOf<UGameplayEffect>,FGlobalAppliedEffectList> AppliedEffects;
	
	// UPROPERTY здесь не нужен, так как TWeakObjectPtr не сериализуется движком по умолчанию таким образом.
	TArray<TWeakObjectPtr<UCoreAbilitySystemComponent>> RegisteredASCs;
};
