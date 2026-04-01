// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectPawnData.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"

#include "ProjectPawnExtensionComponent.generated.h"

#define PROJ_API MODULARPROJECT_API

class UCoreAbilitySystemComponent;

//TODO Create CoreAbilitySystemComponent in CoreGAS plugin

UCLASS(MinimalAPI)
class UProjectPawnExtensionComponent : public UPawnComponent , public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	PROJ_API UProjectPawnExtensionComponent(const FObjectInitializer& OI);
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	static PROJ_API const FName NAME_ActorFeatureName;
	
	//~ Begin IGameFrameworkInitStateInterface
	virtual FName GetFeatureName() const override;
	PROJ_API virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	PROJ_API virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	PROJ_API virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	PROJ_API virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface
	
	UFUNCTION(BlueprintPure,Category="Project|Pawn")
	static UProjectPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor){return Actor?Actor->FindComponentByClass<UProjectPawnExtensionComponent>():nullptr;}
	
	template<class T>
	const T* GetPawnData() const {return Cast<T>(PawnData);}
	
	PROJ_API void SetPawnData(const UProjectPawnData* InPawnData);
	
	UFUNCTION(BlueprintPure,Category="Project|Pawn")
	UCoreAbilitySystemComponent* GetCoreAbilitySystemComponent() const {return AbilitySystemComponent;}
	
	PROJ_API void InitializeAbilitySystem(UCoreAbilitySystemComponent* InASC,AActor* InOwnerActor);
	
	PROJ_API void UnInitializeAbilitySystem();
	
	PROJ_API void HandleControllerChanged();
	
	PROJ_API void HandlePlayerStateReplicated();
	
	PROJ_API void SetupPlayerInputComponent();
	
	PROJ_API void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);
	
	PROJ_API void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);
	
protected:
	
	PROJ_API virtual void OnRegister() override;
	PROJ_API virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION()
	PROJ_API void OnRep_PawnData();
	
	FSimpleMulticastDelegate OnAbilitySystemInitialized;
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;
	
	UPROPERTY(EditInstanceOnly,ReplicatedUsing=OnRep_PawnData,Category="Project|Pawn")
	TObjectPtr<const UProjectPawnData> PawnData;
	
	UPROPERTY(Transient)
	TObjectPtr<UCoreAbilitySystemComponent> AbilitySystemComponent;
};

#undef PROJ_API