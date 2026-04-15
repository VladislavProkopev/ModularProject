// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectCharacter.h"

#include "CoreGameplayTags.h"
#include "CoreInputComponent.h"
#include "EditorCategoryUtils.h"
#include "ProjectPawnExtensionComponent.h"


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

