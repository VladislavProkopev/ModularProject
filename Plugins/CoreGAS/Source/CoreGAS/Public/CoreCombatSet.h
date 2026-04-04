// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"
#include "CoreAttributeSet.h"
#include "CoreCombatSet.generated.h"

class UObject;
struct FFrame;

/**
 * 
 */
UCLASS(BlueprintType)
class COREGAS_API UCoreCombatSet : public UCoreAttributeSet
{
	GENERATED_BODY()
	
public:
	UCoreCombatSet();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	ATTRIBUTE_ACCESSORS(ThisClass,BaseDamage);
	ATTRIBUTE_ACCESSORS(ThisClass,Health);
	ATTRIBUTE_ACCESSORS(ThisClass,MaxHealth);
	ATTRIBUTE_ACCESSORS(ThisClass,Shield);
	ATTRIBUTE_ACCESSORS(ThisClass,MaxShield);
	
protected:
	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_Shield(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_MaxShield(const FGameplayAttributeData& OldValue);
	
private:
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_BaseDamage,Category="GAS|CombatAttributeSet",Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_BaseDamage,Category="GAS|CombatAttributeSet",Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_BaseDamage,Category="GAS|CombatAttributeSet",Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_BaseDamage,Category="GAS|CombatAttributeSet",Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Shield;
	
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_BaseDamage,Category="GAS|CombatAttributeSet",Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxShield;
};
