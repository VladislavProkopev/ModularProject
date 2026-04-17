// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectCharacter.h"

#include "CoreGameplayTags.h"
#include "CoreInputComponent.h"
#include "EditorCategoryUtils.h"
#include "ProjectHeroComponent.h"
#include "ProjectPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"


AProjectCharacter::AProjectCharacter()
{
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;
	
	PawnExtensionComponent = CreateDefaultSubobject<UProjectPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
}


void AProjectCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AProjectCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReason);
}

void AProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UProjectPawnExtensionComponent* PawnExtensionComp = UProjectPawnExtensionComponent::FindPawnExtensionComponent(this))
	{
		PawnExtensionComp->SetupPlayerInputComponent(PlayerInputComponent);
	}

	if (UProjectHeroComponent* HeroComp = UProjectHeroComponent::FindHeroComponent(this))
	{
		HeroComp->InitializePlayerInput(PlayerInputComponent);
	}
}

