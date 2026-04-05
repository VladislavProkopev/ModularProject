// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectPlayerState.h"
#include "CoreAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

AProjectPlayerState::AProjectPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UCoreAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	SetNetUpdateFrequency(100.f);
}

UAbilitySystemComponent* AProjectPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
