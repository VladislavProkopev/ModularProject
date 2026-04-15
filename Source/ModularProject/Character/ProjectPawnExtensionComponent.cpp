// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "ModularCore/Public/CoreGameplayTags.h"
#include "ProjectPawnData.h"
#include "CoreAbilitySystemComponent.h"
#include "CoreInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ProjectCharacter.h"
#include "ProjectPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ProjectPawnExtensionComponent)

DEFINE_LOG_CATEGORY_STATIC(Log_ProjectPawnExtensionComponent, All, All);

class FLifetimeProperty;
class UActorComponent;

const FName UProjectPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");


// Sets default values for this component's properties
UProjectPawnExtensionComponent::UProjectPawnExtensionComponent(const FObjectInitializer& OI) : Super(OI)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	PawnData = nullptr;
	AbilitySystemComponent = nullptr;
}


void UProjectPawnExtensionComponent::SetPawnData(const UProjectPawnData* InPawnData)
{
	checkf(InPawnData,TEXT("PawnData in UProjectPawnExtensionComponent::SetPawnData - Invalid!!!"));
	
	if (!GetOwner()->HasAuthority()) return;

	if (PawnData)
	{
		UE_LOG(Log_ProjectPawnExtensionComponent, Error, TEXT("Trying to set PawnData [%s] on pawn [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(GetOwner()), *GetNameSafe(PawnData));
		return;
	}
	PawnData = InPawnData;
	MARK_PROPERTY_DIRTY_FROM_NAME(UProjectPawnExtensionComponent,PawnData,this);
	CheckDefaultInitialization();
	
}

void UProjectPawnExtensionComponent::InitializeAbilitySystem(UCoreAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	if (AbilitySystemComponent == InASC) return;

	check(InASC);
	check(InOwnerActor);

	APawn* Pawn = GetPawnChecked<APawn>();
	AActor* ExistingAvatar = InASC->GetAvatarActor();

	if ((ExistingAvatar != nullptr) && (ExistingAvatar != Pawn))
	{
		ensure(!ExistingAvatar->HasAuthority());

		if (UProjectPawnExtensionComponent* OtherExtension = ExistingAvatar->FindComponentByClass<UProjectPawnExtensionComponent>())
		{
			OtherExtension->UnInitializeAbilitySystem();
		}
	}

	if (AbilitySystemComponent)
	{
		UnInitializeAbilitySystem();
	}

	AbilitySystemComponent = InASC;
	InASC->InitAbilityActorInfo(InOwnerActor, Pawn);

	if (ensureMsgf(PawnData, TEXT("PawnData is null on %s!"), *GetNameSafe(Pawn)))
	{
		if (PawnData->TagRelationshipMapping)
		{
			InASC->SetTargetRelationshipMapping(PawnData->TagRelationshipMapping);
		}

		if (Pawn->HasAuthority())
		{
			//TODO ChangeNaming when test's finished
			if (IsValid(PawnData->StartupAbilityTEST))
			{
				FGameplayAbilitySpec AbilitySpec(PawnData->StartupAbilityTEST, 1.f, INDEX_NONE, Pawn);
				InASC->GiveAbility(AbilitySpec);
			}

			if (IsValid(PawnData->DefaultAttributesGE))
			{
				FGameplayEffectContextHandle EffectContext = InASC->MakeEffectContext();
				EffectContext.AddSourceObject(Pawn);

				FGameplayEffectSpecHandle SpecHandle = InASC->MakeOutgoingSpec(PawnData->DefaultAttributesGE,1.f,EffectContext);
				if (SpecHandle.IsValid())
				{
					InASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				}
			}
		}
	}
	
	OnProjectPawnReadyDelegate.Broadcast();
	//if (AbilitySystemComponent == InASC) return;


	//checkf(InASC, TEXT("In AbilitySystem Component in UProjectPawnExtensionComponent::InitializeAbilitySystem - Invalid!!!"));
	//checkf(InOwnerActor, TEXT("In OwnerActor in UProjectPawnExtensionComponent::InitializeAbilitySystem - Invalid!!!"));


	//APawn* Pawn = GetPawn<APawn>();
	//checkf(Pawn,TEXT("Pawn in UProjectPawnExtensionComponent::InitializeAbilitySystem - Invalid!!!"));
	//
	//if (AbilitySystemComponent)
	//{
	//	UnInitializeAbilitySystem();
	//}

	//AbilitySystemComponent = InASC;
	//InASC->InitAbilityActorInfo(InOwnerActor, Pawn);

	//if (PawnData->TagRelationshipMapping)
	//{
	//	InASC->SetTargetRelationshipMapping(PawnData->TagRelationshipMapping);
	//}

	//if (Pawn->HasAuthority())
	//{
	//	if (PawnData->StartupAbilityTEST->IsValidLowLevel())
	//	{
	//		/*TODO забыл как выдаются способности, посмотреть позже
	//		FGameplayAbilitySpec Spec = InASC->MakeOutgoingSpec();
	//		InASC->GiveAbility(PawnData->StartupAbilityTEST)
	//		*/
	//	}
	//	//Применяем стартовый геймплей эффект для инициализации аттрибутов
	//	if (PawnData && PawnData->DefaultAttributesGE)
	//	{
	//		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	//		EffectContext.AddSourceObject(Pawn);
	//		
	//		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(PawnData->DefaultAttributesGE,1,EffectContext);
	//		if (SpecHandle.IsValid())
	//		{
	//			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	//		}
	//	}
	//}
	//
	//OnProjectPawnReadyDelegate.Broadcast();
}

void UProjectPawnExtensionComponent::UnInitializeAbilitySystem()
{
	if (!AbilitySystemComponent) return;

	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		if (AProjectCharacter* Character = GetPawn<AProjectCharacter>())
		{
			if (UCoreInputComponent* CoreIC = Cast<UCoreInputComponent>(Character->InputComponent))
			{
				CoreIC->RemoveBinds(AbilityInputBindingsHandles);
				AbilityInputBindingsHandles.Empty();
			}
			if (const APlayerController* PC = Cast<APlayerController>(Character->GetController()))
			{
				if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
				{
					Subsystem->RemoveMappingContext(PawnData->DefaultInputContext);
				}
			}
			
			FGameplayTagContainer AbilityTypesToIgnore;
			AbilityTypesToIgnore.AddTag(CoreGameplayTags::Abilities::Ability_Behavior_SurvivesDeath);
			
			AbilitySystemComponent->CancelAbilities(nullptr,&AbilityTypesToIgnore);
			AbilitySystemComponent->ClearAbilityInput();
			AbilitySystemComponent->RemoveAllGameplayCues();

			if (AbilitySystemComponent->GetOwnerActor() != nullptr)
			{
				AbilitySystemComponent->SetAvatarActor(nullptr);
			}
			else
			{
				AbilitySystemComponent->ClearActorInfo();
			}
			
			OnAbilitySystemUninitialized.Broadcast();
		}
	}

	AbilitySystemComponent = nullptr;
}

void UProjectPawnExtensionComponent::HandleControllerChanged()
{
	if (AbilitySystemComponent && (AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>()))
	{
		ensure(AbilitySystemComponent->AbilityActorInfo->OwnerActor == AbilitySystemComponent->GetOwnerActor());
		if (AbilitySystemComponent->GetOwnerActor() == nullptr)
		{
			UnInitializeAbilitySystem();
		}
		else
		{
			AbilitySystemComponent->RefreshAbilityActorInfo();
		}
	}
	CheckDefaultInitialization();
}

void UProjectPawnExtensionComponent::HandlePlayerStateReplicated()
{
	CheckDefaultInitialization();
}

void UProjectPawnExtensionComponent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UCoreInputComponent* InputComponent = Cast<UCoreInputComponent>(PlayerInputComponent);
	if (!InputComponent) return;
	
	const UProjectPawnData* CurrentPawnData = GetPawnData<UProjectPawnData>();
	if (!CurrentPawnData || !CurrentPawnData->InputConfig) return;
	
	InputComponent->BindAbilityActions(
		CurrentPawnData->InputConfig,
		this,&ThisClass::Input_AbilityInputTagPressed,
		&ThisClass::Input_AbilityInputTagReleased,
		AbilityInputBindingsHandles);
	
	
	CheckDefaultInitialization();
}

void UProjectPawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(
	FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}
	
	if (AbilitySystemComponent)
	{
		Delegate.Execute();
	}
}

void UProjectPawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}
}

void UProjectPawnExtensionComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityInputTagPressed(InputTag);
	}
}

void UProjectPawnExtensionComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityInputTagReleased(InputTag);
	}
}

void UProjectPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("ProjectPawnExtensionComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> PawnExtensionComponents;
	Pawn->GetComponents(UProjectPawnExtensionComponent::StaticClass(), PawnExtensionComponents);
	ensureAlwaysMsgf((PawnExtensionComponents.Num() == 1), TEXT(" Only one ProjectPawnExtensionComponent should be exist on [%s]."), *GetNameSafe(GetOwner()));

	RegisterInitStateFeature();
}

void UProjectPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnInitializeAbilitySystem();
	UnregisterInitStateFeature();

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

	//Listen for changes to all features
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	//Notifies state manager that we have spawned, then try rest of default initializations
	ensure(TryToChangeInitState(CoreGameplayTags::GameFeaturesDataStates::InitState_Spawned));
	CheckDefaultInitialization();
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
	check(Manager);
	
	APawn* Pawn = GetPawn<APawn>();
	if (!CurrentState.IsValid() && DesiredState == CoreGameplayTags::GameFeaturesDataStates::InitState_Spawned)
	{
		if (Pawn)
		{
			return true;
		}
	}
	if (CurrentState == CoreGameplayTags::GameFeaturesDataStates::InitState_Spawned && DesiredState == CoreGameplayTags::GameFeaturesDataStates::InitState_DataAvaliable)
	{
		if (!PawnData)
		{
			return false;
		}
		
		const bool bHasAuthority = Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

		if (bHasAuthority || bIsLocallyControlled)
		{
			//Check for being possessed by controller
			if (!GetController<AController>())
			{
				return false;
			}
		}
		return true;
	}
	else if (CurrentState == CoreGameplayTags::GameFeaturesDataStates::InitState_DataInitialized && DesiredState == CoreGameplayTags::GameFeaturesDataStates::InitState_GameplayReady)
	{
		return true;		
	}
	return false;
}

void UProjectPawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (DesiredState == CoreGameplayTags::GameFeaturesDataStates::InitState_DataInitialized)
	{
		//This is currently all handled by other components listening to this state change
	}
}

void UProjectPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	//If another feature is now in DataAvaliable, see if we shoud transitition to DataInitialized
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		if (Params.FeatureState == CoreGameplayTags::GameFeaturesDataStates::InitState_DataAvaliable)
		{
			CheckDefaultInitialization();
		}
	}
}

void UProjectPawnExtensionComponent::CheckDefaultInitialization()
{
	//Before checking our progress, try progressing any other features we might depend on
	CheckDefaultInitializationForImplementers();
	
	static const TArray<FGameplayTag> StateChain = {CoreGameplayTags::GameFeaturesDataStates::InitState_Spawned,
		CoreGameplayTags::GameFeaturesDataStates::InitState_DataAvaliable,
		CoreGameplayTags::GameFeaturesDataStates::InitState_DataInitialized,
		CoreGameplayTags::GameFeaturesDataStates::InitState_GameplayReady};
	
	//This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
	
	
	//TODO Перенести логику с новосозданный компонент типа GASFeaturesComponent или что то типа такого
	/*APawn* Pawn = GetPawn<APawn>();
	if (!Pawn || !PawnData) return;
	
	AProjectPlayerState* PlayerState = Cast<AProjectPlayerState>(Pawn->GetPlayerState());
	if (!PlayerState) return;
	
	UCoreAbilitySystemComponent* ASC = Cast<UCoreAbilitySystemComponent>(PlayerState->GetAbilitySystemComponent());
	if (ASC)
	{
		InitializeAbilitySystem(ASC, PlayerState);
	}*/
}



