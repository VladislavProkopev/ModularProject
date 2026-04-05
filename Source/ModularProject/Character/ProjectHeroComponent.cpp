// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectHeroComponent.h"


UProjectHeroComponent::UProjectHeroComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

}


void UProjectHeroComponent::BeginPlay()
{
	Super::BeginPlay();

}



