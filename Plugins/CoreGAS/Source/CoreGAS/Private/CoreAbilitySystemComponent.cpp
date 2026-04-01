#include "CoreAbilitySystemComponent.h"


UCoreAbilitySystemComponent::UCoreAbilitySystemComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}


void UCoreAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


