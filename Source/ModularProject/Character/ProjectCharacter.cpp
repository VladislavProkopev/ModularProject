// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectCharacter.h"


AProjectCharacter::AProjectCharacter()
{
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = true;

}


void AProjectCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

