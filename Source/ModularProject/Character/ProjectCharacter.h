// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjectCharacter.generated.h"

struct FInputActionValue;
class UProjectPawnExtensionComponent;

UCLASS()
class MODULARPROJECT_API AProjectCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AProjectCharacter();
	virtual void BeginPlay() override;
	
	UProjectPawnExtensionComponent* GetPawnExtensionComponent() const {return PawnExtensionComponent;}
	
	virtual void PreInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
private:
	UPROPERTY(EditDefaultsOnly,Category="Project|GameFeatures")
	TObjectPtr<UProjectPawnExtensionComponent> PawnExtensionComponent;
	
};
