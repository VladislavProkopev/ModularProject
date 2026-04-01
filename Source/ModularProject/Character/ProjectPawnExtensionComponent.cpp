// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectPawnExtensionComponent.h"


// Sets default values for this component's properties
UProjectPawnExtensionComponent::UProjectPawnExtensionComponent(const FObjectInitializer& OI) : Super(OI)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UProjectPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	
}



