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

void AProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UCoreInputComponent* CoreInputComponent = Cast<UCoreInputComponent>(PlayerInputComponent);
	if (!CoreInputComponent || !PawnExtensionComponent) return;
	
	const UProjectPawnData* PawnData = PawnExtensionComponent->GetPawnData<UProjectPawnData>();
	if (!PawnData || !PawnData->InputConfig) return;
	
	for (const FCoreInputAction& Action : PawnData->InputConfig->NativeInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			CoreInputComponent->BindNativeAction(
				PawnData->InputConfig,
				CoreGameplayTags::NativeInput::Input_Move,
				ETriggerEvent::Triggered,
				this,
				&ThisClass::Move,
				true);
			
			CoreInputComponent->BindNativeAction(
				PawnData->InputConfig,
				CoreGameplayTags::NativeInput::Input_Look,
				ETriggerEvent::Triggered,
				this,
				&ThisClass::Look,
				true);
			
		}
	}
	

}

void AProjectCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MoveVector = Value.Get<FVector2D>();
	const FRotator MoveRotation(0.f,Controller->GetControlRotation().Yaw,0.f);
	
	if (MoveVector.X != 0.f)
	{
		const FVector MovementDirection = MoveRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(MovementDirection, MoveVector.X);
	}
	if (MoveVector.Y != 0.f)
	{
		const FVector MovementDirection = MoveRotation.RotateVector(FVector::RightVector);
		AddMovementInput(MovementDirection, MoveVector.Y);
	}
}

void AProjectCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();

	if (LookVector.X != 0.f)
	{
		AddControllerYawInput(LookVector.X);
	}
	if (LookVector.Y != 0.f)
	{
		AddControllerPitchInput(LookVector.Y);
	}
}

