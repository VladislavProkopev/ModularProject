// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreAttributeSet.h"
#include "CoreAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CoreAttributeSet)

class UWorld;

UCoreAttributeSet::UCoreAttributeSet()
{
}

UWorld* UCoreAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);
	
	return Outer->GetWorld();
}

UCoreAbilitySystemComponent* UCoreAttributeSet::GetCoreAbilitySystemComponent() const
{
	return Cast<UCoreAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
