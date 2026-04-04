// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreCombatSet.h"

#include "Net/UnrealNetwork.h"

UCoreCombatSet::UCoreCombatSet()
{
}

void UCoreCombatSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_OwnerOnly;
	Params.RepNotifyCondition = REPNOTIFY_Always;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass,BaseDamage,Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass,Health,Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass,MaxHealth,Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass,Shield,Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass,MaxShield,Params);
}

void UCoreCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,BaseDamage,OldValue);
}

void UCoreCombatSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Health,OldValue);
}

void UCoreCombatSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,MaxHealth,OldValue);
}

void UCoreCombatSet::OnRep_Shield(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,Shield,OldValue);
}

void UCoreCombatSet::OnRep_MaxShield(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass,MaxShield,OldValue);
}
