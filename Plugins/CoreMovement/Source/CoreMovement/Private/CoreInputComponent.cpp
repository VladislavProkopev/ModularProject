// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreInputComponent.h"
#include "EnhancedInputComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CoreInputComponent)

// Sets default values for this component's properties
UCoreInputComponent::UCoreInputComponent(const FObjectInitializer& OI) : Super(OI)
{

}

void UCoreInputComponent::AddInputMappings(const UCoreInputConfig* InputConfig,
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);
}

void UCoreInputComponent::RemoveInputMappings(const UCoreInputConfig* InputConfig,
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);
}

void UCoreInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Hadle : BindHandles)
	{
		RemoveBindingByHandle(Hadle);
	}
	BindHandles.Reset();
}






