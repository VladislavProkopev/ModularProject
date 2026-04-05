// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/GameFrameworkInitStateInterface.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameFeatures/GameFeatureAction_AddInputMappingContext.h"
#include "Components/PawnComponent.h"
#include "ProjectHeroComponent.generated.h"

#define MOD_API MODULARPROJECT_API

namespace EEndPlayReason{ enum Type : int;}
struct FLoadedMappableConfigPair; //??
struct FMappableConfigPair; //??

class UGameFrameworkComponentManager;
class UInputComponent;
class UCoreInputConfig;
class UObject;
struct FActorInitStateChangedParams;
struct FFrame;
struct FGameplayTag;
struct FInputActionValue;



UCLASS(MinimalAPI,Blueprintable,Meta = (BlueprintSpawnableComponent))
class UProjectHeroComponent : public UPawnComponent , public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	MOD_API UProjectHeroComponent(const FObjectInitializer& ObjectInitializer);
	
	static UProjectHeroComponent* FindHeroComponent(const AActor* Actor){return (Actor ? Actor->FindComponentByClass<UProjectHeroComponent>() : nullptr);}
	
	/*TODO Realize logic When CoreCameraComponent will be created
	* //Overrides the camera from an active gameplay ability
	*UE_API void SetAbilityCameraMode(TSubclassOf<ULyraCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle);
	* //Clears the camera override if it is set
	*UE_API void ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle);
	*/
	
	/** Adds mode-specific input config */
	MOD_API void AddAdditionalInputConfig(const UCoreInputConfig* InputConfig);
	/** Removes a mode-specific input config if it has been added */
	MOD_API void RemoveAdditionalInputConfig(const UCoreInputConfig* InputConfig);
	/** True if this is controlled by a real player and has progressed far enough in initialization where additional input bindings can be added */
	MOD_API bool IsReadyToBindInputs() const;

	/** The name of the extension event sent via UGameFrameworkComponentManager when ability inputs are ready to bind */
	static MOD_API const FName NAME_BindInputsNow;
	/** The name of this component-implemented feature */
	static MOD_API const FName NAME_ActorFeatureName;
	
	//~ Begin IGameFrameworkInitStateInterface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	MOD_API virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	MOD_API virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	MOD_API virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	MOD_API virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface
protected:
	
	MOD_API virtual void BeginPlay() override;
	MOD_API virtual void EndPlay(const EEndPlayReason EndPlayReason) override;
	MOD_API virtual void OnRegister() override;
	
	MOD_API virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);
	
	MOD_API void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	MOD_API void Input_AbilityInputTagReleased(FGameplayTag InputTag);
	
	MOD_API void Input_Move(const FInputActionValue& InputValue);
	MOD_API void Input_LookMouse(const FInputActionValue& InputValue);
	MOD_API void Input_Crouch(const FInputActionValue& InputValue);

	//TODO UE_API TSubclassOf<ULyraCameraMode> DetermineCameraMode() const;
	
protected:
	UPROPERTY(EditAnywhere)
	TArray<FInputMappingContextAndPriority> DefaultInputMappings;

	/*TODO
	 *Camera mode set by an ability.
	 *UPROPERTY()
	 *TSubclassOf<ULyraCameraMode> AbilityCameraMode;
	 * Spec handle for the last ability to set a camera mode.
	 * FGameplayAbilitySpecHandle AbilityCameraModeOwningSpecHandle;
	 */
	
	/** True when player input bindings have been applied, will never be true for non - players */
	bool bReadyToBindInputs;
};

#undef MOD_API