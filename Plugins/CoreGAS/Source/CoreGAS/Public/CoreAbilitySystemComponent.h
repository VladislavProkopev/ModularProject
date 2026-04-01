// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"

#include "CoreAbilitySystemComponent.generated.h"

#define GAS_API COREGAS_API

class AActor;
class UGameplayAbility;
//TODO CoreAbilityTagRelationshipMapping
class OUbject;
struct FFrame;
struct FGameplayAbilityTargetDataHandle;



UCLASS(MinimalAPI)
class  UCoreAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	GAS_API UCoreAbilitySystemComponent(const FObjectInitializer& OI);

	//~ Begin UActorComponentInterface
	GAS_API virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UActorComponentInterface
	
	GAS_API virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	
	typedef TFunctionRef<bool(const UCoreGameplayAbility* CoreAbility, FGameplayAbilitySpecHandle Handle)> TShouldCancelAbilityFunc;
	GAS_API void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility);
	
	GAS_API void CancelInputActivatedAbilities(bool bReplicateCancelAbility);
	
	GAS_API void AbilityInputTagPressed(const FGameplayTag& InputTag);
	GAS_API void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
	GAS_API void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	GAS_API void ClearAbilityInput();
	
	//TODO ECoreAbilityActivationGroup in CoreGameplayAbility
	GAS_API bool IsActivationGroupBlocked(ECoreAbilityActivationGroup Group) const;
	GAS_API void AddAbilityToActivationGroup(ECoreAbilityActivationGroup Group, UCoreGameplayAbility CoreAbility);
	GAS_API void RemoveAbilityFromActivationGroup(ECoreAbilityActivationGroup Group, UCoreGameplayAbility CoreAbility);
	GAS_API void CancelActivationGroupAbilities(ECoreAbilityActivationGroup Group, UCoreGameplayAbility CoreAbility,bool bReplicateCancelAbility);
	
	GAS_API void AddDynamicTagGameplayEffect(const FGameplayTag& Tag);
	
	GAS_API void RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag);
	
	GAS_API void GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle,FGameplayAbilityActivationInfo ActivationInfo,FGameplayAbilityTargetDataHandle& OutTargetDataHandle);
	//TODO UCoreAbilityTagRelationshipMapping
	GAS_API void SetTargetRelationshipMapping(UCoreAbilityTagRelationshipMapping* NewMapping);
	
	GAS_API void GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags,FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const;
	
	GAS_API void TryActivateAlilitiesOnSpawn(); 
protected:
	virtual void BeginPlay() override;
	GAS_API virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	GAS_API virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
	
	GAS_API virtual void NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability) override;
	GAS_API virtual void NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason) override;
	GAS_API virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;
	GAS_API virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;
	GAS_API virtual void HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bCanBeCanceled) override;
	
	UFUNCTION(Client,Unreliable)
	GAS_API void ClientNotifyAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);
	
	GAS_API void HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);
	
	UPROPERTY()
	TObjectPtr<UCoreAbilityTagRelationshipMapping> TagRelationshipMapping;
	
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;
	
	int32 ActivationGroupCounts [(uint8) ECoreAbilityActivationGroup::MAX];
};

#undef GAS_API