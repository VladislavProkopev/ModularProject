// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectPawnExtensionComponent.h"
#include "CoreAbilitySystemComponent.h"
#include "ProjectPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

// Sets default values for this component's properties
UProjectPawnExtensionComponent::UProjectPawnExtensionComponent(const FObjectInitializer& OI) : Super(OI)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}


void UProjectPawnExtensionComponent::SetPawnData(const UProjectPawnData* InPawnData)
{
	checkf(InPawnData,TEXT("PawnData in UProjectPawnExtensionComponent::SetPawnData - Invalid!!!"));
	if (GetOwner()->HasAuthority())
	{
		PawnData = InPawnData;
		MARK_PROPERTY_DIRTY_FROM_NAME(UProjectPawnExtensionComponent,PawnData,this);
		CheckDefaultInitialization();
	}
}

void UProjectPawnExtensionComponent::InitializeAbilitySystem(UCoreAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	APawn* Pawn = GetPawn<APawn>();
	
	InASC->InitAbilityActorInfo(InOwnerActor, Pawn);

	if (PawnData->TagRelationshipMapping)
	{
		InASC->SetTargetRelationshipMapping(PawnData->TagRelationshipMapping);
	}

	if (Pawn->HasAuthority())
	{
		if (PawnData->StartupAbilityTEST->IsValidLowLevel())
		{
			/*TODO забыл как выдаются способности, посмотреть позже
			FGameplayAbilitySpec Spec = InASC->MakeOutgoingSpec();
			InASC->GiveAbility(PawnData->StartupAbilityTEST)
			*/
		}
	}
	OnProjectPawnReadyDelegate.Broadcast();
}

void UProjectPawnExtensionComponent::UnInitializeAbilitySystem()
{
}

void UProjectPawnExtensionComponent::HandleControllerChanged()
{
}

void UProjectPawnExtensionComponent::HandlePlayerStateReplicated()
{
	CheckDefaultInitialization();
}

void UProjectPawnExtensionComponent::SetupPlayerInputComponent()
{
}

void UProjectPawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(
	FSimpleMulticastDelegate::FDelegate Delegate)
{
}

void UProjectPawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
}

void UProjectPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();
}

void UProjectPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UProjectPawnExtensionComponent::OnRep_PawnData()
{
	CheckDefaultInitialization();
}

// Called when the game starts
void UProjectPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UProjectPawnExtensionComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// AAA Подход: Использование Push-модели для минимизации нагрузки на Game Thread.
	// Свойство не будет опрашиваться движком каждый тик.
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true; 

	// Используем FAST версию макроса для оптимизации
	DOREPLIFETIME_WITH_PARAMS_FAST(UProjectPawnExtensionComponent, PawnData, Params);
}

FName UProjectPawnExtensionComponent::GetFeatureName() const
{
	return IGameFrameworkInitStateInterface::GetFeatureName();
}

bool UProjectPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	return IGameFrameworkInitStateInterface::CanChangeInitState(Manager, CurrentState, DesiredState);
}

void UProjectPawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	IGameFrameworkInitStateInterface::HandleChangeInitState(Manager, CurrentState, DesiredState);
}

void UProjectPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	IGameFrameworkInitStateInterface::OnActorInitStateChanged(Params);
}

void UProjectPawnExtensionComponent::CheckDefaultInitialization()
{
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn || !PawnData) return;
	
	AProjectPlayerState* PlayerState = Cast<AProjectPlayerState>(Pawn->GetPlayerState());
	if (!PlayerState) return;
	
	UCoreAbilitySystemComponent* ASC = Cast<UCoreAbilitySystemComponent>(PlayerState->GetAbilitySystemComponent());
	if (ASC)
	{
		InitializeAbilitySystem(ASC, PlayerState);
	}
}



