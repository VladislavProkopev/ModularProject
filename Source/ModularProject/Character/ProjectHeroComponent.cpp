// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectHeroComponent.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Logging/MessageLog.h"
//TODO CoreLogChannels.h
#include "EnhancedInputSubsystems.h"
#include "ProjectPawnExtensionComponent.h"
#include "ProjectPawnData.h"
#include "ProjectCharacter.h"
#include "CoreAbilitySystemComponent.h"
#include "CoreInputConfig.h"
#include "CoreInputComponent.h"
//TODO CoreCameraComponent
#include "CoreGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include "PlayerMappableInputConfig.h"
//TODO CoreCameraMode
//TODO UserSettings/EnhancedInputUserSettings
#include "InputMappingContext.h"
#include "ProjectPlayerController.h"
#include "ProjectPlayerState.h"
#include "UserSettings/EnhancedInputUserSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ProjectHeroComponent)

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif

namespace ProjectHero
{
	static const float LookYawRate = 300.0f;
	static const float LookPitchRate = 165.0f;
}

const FName UProjectHeroComponent::NAME_BindInputsNow("BindInputsNow");
const FName UProjectHeroComponent::NAME_ActorFeatureName("Hero");

DEFINE_LOG_CATEGORY_STATIC(Log_ProjectHeroComponent, All, All);

UProjectHeroComponent::UProjectHeroComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

}

void UProjectHeroComponent::AddAdditionalInputConfig(const UCoreInputConfig* InputConfig)
{
	TArray<uint32> BindHandles;
	
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}
	
	const APlayerController* PC = GetController<APlayerController>();
	checkf(PC,TEXT("APlayerController is Invalid in UProjectHeroComponent::AddAdditionalInputConfig"));
	
	const ULocalPlayer* LP = PC->GetLocalPlayer();
	checkf(LP,TEXT("ULocalPlayer is Invalid in UProjectHeroComponent::AddAdditionalInputConfig"));
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	checkf(Subsystem,TEXT("UEnhancedInputLocalPlayerSubsystem is Invalid in UProjectHeroComponent::AddAdditionalInputConfig"));

	if (const UProjectPawnExtensionComponent* PawnExtComp = UProjectPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		UCoreInputComponent* CoreIC = Pawn->FindComponentByClass<UCoreInputComponent>();
		if (ensureMsgf(CoreIC,TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UCoreInputComponent")))
		{
			CoreIC->BindAbilityActions(InputConfig,this,&ThisClass::Input_AbilityInputTagPressed,&ThisClass::Input_AbilityInputTagReleased,BindHandles);
		}
	}
}

void UProjectHeroComponent::RemoveAdditionalInputConfig(const UCoreInputConfig* InputConfig)
{
	//Тут кода даже в лире нет
}

bool UProjectHeroComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

bool UProjectHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState) const
{
	check(Manager);
	
	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == CoreGameplayTags::GameFeaturesDataStates::InitState_Spawned)
	{
		//As long as we have a real pawn, let us transition
		if (Pawn)
		{
			return true;
		}
	}
	else if( CurrentState == CoreGameplayTags::GameFeaturesDataStates::InitState_Spawned && DesiredState == CoreGameplayTags::GameFeaturesDataStates::InitState_DataAvaliable)
	{
		//The player state is required
		if (!GetPlayerState<AProjectPlayerState>())
		{
			return false;
		}
		
		//If we're authoruty or autonomous, we need to wait for a controller with registered ownership of the player state.
		if (Pawn->GetLocalRole() == ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();
			
			const bool bHasControllerPairedWithPS = (Controller != nullptr) && \
			(Controller->PlayerState != nullptr) && \
			(Controller->PlayerState->GetOwner() == Controller);

			if (!bHasControllerPairedWithPS)
			{
				return false;
			}
		}
		
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();

		if (bIsLocallyControlled && !bIsBot)
		{
			AProjectPlayerController* ProjectPC = GetController<AProjectPlayerController>();
			
			//The input component and local player is required when locally controlled.
			if (!Pawn->InputComponent || !ProjectPC || !ProjectPC->GetLocalPlayer())
			{
				return false;
			}
		}
		
		return true;
	}
	else if (CurrentState == CoreGameplayTags::GameFeaturesDataStates::InitState_DataAvaliable &&
		DesiredState == CoreGameplayTags::GameFeaturesDataStates::InitState_DataInitialized)
	{
		// Wait for player state and extension component
		AProjectPlayerState* ProjectPS = GetPlayerState<AProjectPlayerState>();
		
		return ProjectPS && Manager->HasFeatureReachedInitState(Pawn,
			UProjectPawnExtensionComponent::NAME_ActorFeatureName,
			CoreGameplayTags::GameFeaturesDataStates::InitState_DataInitialized);
	}
	else if (CurrentState == CoreGameplayTags::GameFeaturesDataStates::InitState_DataInitialized &&
		DesiredState == CoreGameplayTags::GameFeaturesDataStates::InitState_GameplayReady)
	{
		// TODO add ability initialization checks?
		// TODO подразумевает инициализацию ГАС на сервере которая тут не реализована, позже вернуться и решить проблему
		// TODO Чтобы не было гонки состояний и правильная инициализация ГАС
		return true;
	}
	
	return false;
}

void UProjectHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState)
{
	if (CurrentState == CoreGameplayTags::GameFeaturesDataStates::InitState_DataAvaliable && DesiredState == CoreGameplayTags::GameFeaturesDataStates::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		AProjectPlayerState* ProjectPS = GetPlayerState<AProjectPlayerState>();
		if (!ensure(Pawn&&ProjectPS))
		{
			return;
		}
		
		const UProjectPawnData* PawnData = nullptr;

		if (UProjectPawnExtensionComponent* PawnExtComp = UProjectPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UProjectPawnData>();
			
			//The player state holds the presistent data for this player(state that persist across deaths and multiple pawns).
			//The ability system component and attribute sets live on player state.
			PawnExtComp->InitializeAbilitySystem(ProjectPS->GetCoreAbilitySystemComponent(),ProjectPS);
		}
		
		if (AProjectPlayerController* ProjectPC = GetController<AProjectPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}
		
		//TODO CameraComponent
		/*if (PawnData)
		{
			if (UCoreCameraComponent* CameraComponent = UCoreCameraComponent::FindCameraComponent(Pawn))
			{
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this,&ThisClass::DetermineCameraMode);
			}
		}*/
	}
}

void UProjectHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UProjectPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == CoreGameplayTags::GameFeaturesDataStates::InitState_DataInitialized)
		{
			//If the extension component says all all other components are initialized, try to progress to next state
			CheckDefaultInitialization();
		}
	}
}

void UProjectHeroComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = {
		CoreGameplayTags::GameFeaturesDataStates::InitState_Spawned,
		CoreGameplayTags::GameFeaturesDataStates::InitState_DataAvaliable,
		CoreGameplayTags::GameFeaturesDataStates::InitState_DataInitialized,
		CoreGameplayTags::GameFeaturesDataStates::InitState_GameplayReady
	};
	
	ContinueInitStateChain(StateChain);
}

void UProjectHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	Super::EndPlay(EndPlayReason);
}

void UProjectHeroComponent::OnRegister()
{
	Super::OnRegister();
	
	if (!GetPawn<APawn>())
	{
		UE_LOG(Log_ProjectHeroComponent, Error, TEXT("[UProjectHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));
		
#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("ProjectHeroComponent", "NotOnPawnError","has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName("ProjectHeroComponent");
			
			FMessageLog(HeroMessageLogName).Error()->AddToken(FUObjectToken::Create(this,FText::FromString(GetNameSafe(this))))->AddToken(
				FTextToken::Create(Message));
			
			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
	else
	{
		//Register with the init state system early, this will work if this is a game world
		RegisterInitStateFeature();
	}
}

void UProjectHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	checkf(PlayerInputComponent,TEXT("PlayerInputComponent is NULL in UProjectHeroComponent::InitializePlayerInput"));
	
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn) return;
	
	const APlayerController* PC = GetController<APlayerController>();
	checkf(PC,TEXT("PlayerController is NULL in UProjectHeroComponent::InitializePlayerInput"));
	
	//TODO Create ProjectLocalPlayer
	ULocalPlayer* LP = PC->GetLocalPlayer();
	checkf(LP,TEXT("LocalPlayer is NULL in UProjectHeroComponent::InitializePlayerInput"));
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	checkf(Subsystem,TEXT("UEnhancedInputLocalPlayerSubsystem is Null in UProjectHeroComponent::InitializePlayerInput"));
	
	Subsystem->ClearAllMappings();

	if (const UProjectPawnExtensionComponent* PawnExtComp = UProjectPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UProjectPawnData* PawnData = PawnExtComp->GetPawnData<UProjectPawnData>())
		{
			if (const UCoreInputConfig* InputConfig = PawnData->InputConfig)
			{
				for (const FInputMappingContextAndPriority& Mapping : DefaultInputMappings)
				{
					if (UInputMappingContext* InputMappingContext = Mapping.InputMapping.LoadSynchronous())
					{
						if (Mapping.bRegisterWithSettings)
						{
							if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
							{
								Settings->RegisterInputMappingContext(InputMappingContext);
							}
							
							FModifyContextOptions Options = {};
							Options.bIgnoreAllPressedKeysUntilRelease = false;
							//Actually add the config to the local player
							Subsystem->AddMappingContext(InputMappingContext,Mapping.Priority,Options);
						}
					}
				}
				
				//The Lyra Input Component has some additional functions to map Gameplay Tags to an Input Action.
				//If you want this functionality but still want to change your input component class, make it a subclass
				//of the UCoreInputComponent or modify this component accordingly.
				
				UCoreInputComponent* CoreIC = Cast<UCoreInputComponent>(PlayerInputComponent);
				if (ensureMsgf(CoreIC,TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UCoreInputComponent or a sublass of it.")))
				{
					//Add the key mappings that may have been set by the player
					CoreIC->AddInputMappings(InputConfig,Subsystem);
					
					//This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
					//be triggered directly by these input actions Triggered events.
					TArray<uint32> BindHandles;
					CoreIC->BindAbilityActions(InputConfig,this,&ThisClass::Input_AbilityInputTagPressed,&ThisClass::Input_AbilityInputTagReleased,BindHandles);
					
					CoreIC->BindNativeAction(InputConfig,CoreGameplayTags::NativeInput::Input_Move,ETriggerEvent::Triggered,this,&ThisClass::Input_Move,false);
					CoreIC->BindNativeAction(InputConfig,CoreGameplayTags::NativeInput::Input_Look,ETriggerEvent::Triggered,this,&ThisClass::Input_LookMouse,false);
				}
			}
		}
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}
	
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC),NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn),NAME_BindInputsNow);
}

void UProjectHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UProjectPawnExtensionComponent* PawnExtComp = UProjectPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UCoreAbilitySystemComponent* CoreASC = PawnExtComp->GetCoreAbilitySystemComponent())
			{
				CoreASC->AbilityInputTagPressed(InputTag);
			}
		}
	}
}

void UProjectHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UProjectPawnExtensionComponent* PawnExtComp = UProjectPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UCoreAbilitySystemComponent* CoreASC = PawnExtComp->GetCoreAbilitySystemComponent())
			{
				CoreASC->AbilityInputTagReleased(InputTag);
			}
		}
	}
}

void UProjectHeroComponent::Input_Move(const FInputActionValue& InputValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	// If the player has attempted to move again then cancel auto running
	if (AProjectPlayerController* LyraController = Cast<AProjectPlayerController>(Controller))
	{
		//TODO ProjectPlayerController
		//LyraController->SetIsAutoRunning(false);
	}
	
	if (Controller)
	{
		const FVector2D Value = InputValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UProjectHeroComponent::Input_LookMouse(const FInputActionValue& InputValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

void UProjectHeroComponent::Input_Crouch(const FInputActionValue& InputValue)
{
	if (AProjectCharacter* Character = GetPawn<AProjectCharacter>())
	{
		//TODO AProjectCharacter
		//Character->ToggleCrouch();
	}
}


void UProjectHeroComponent::BeginPlay()
{
	Super::BeginPlay();
	
	//Listen fot when the pawn extension
	BindOnActorInitStateChanged(UProjectPawnExtensionComponent::NAME_ActorFeatureName,FGameplayTag(),false);
	
	//Notifies that we are done spawning, then try the rest of initialization
	ensure(TryToChangeInitState(CoreGameplayTags::GameFeaturesDataStates::InitState_Spawned));
	CheckDefaultInitialization();
	
	UE_LOG(Log_ProjectHeroComponent,Warning,TEXT("UProjectHeroComponent::BeginPlay()"));
}



