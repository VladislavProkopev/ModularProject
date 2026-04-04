// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreGameplayAbility.h"

UCoreGameplayAbility::UCoreGameplayAbility(const FObjectInitializer& OI) : Super(OI)
{
}

UCoreAbilitySystemComponent* UCoreGameplayAbility::GetCoreAbilitySystemComponentFromActorInfo() const
{
	//TODO Затычка
	return nullptr;
}

APlayerController* UCoreGameplayAbility::GetPlayerControllerFromActorInfo() const
{
	//TODO Затычка
	return nullptr;
}

AController* UCoreGameplayAbility::GetControllerFromActorInfo() const
{
	//TODO Затычка
	return nullptr;
}

ACharacter* UCoreGameplayAbility::GetCharacterFromActorInfo() const
{
	//TODO Затычка
	return nullptr;
}

void UCoreGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec) const
{
}

bool UCoreGameplayAbility::CanChangeActivationGroup(ECoreAbilityActivationGroup NewGroup) const
{
	//TODO Затычка
	return false;
}

bool UCoreGameplayAbility::ChangeActivationGroup(ECoreAbilityActivationGroup NewGroup)
{
	//TODO Затычка
	return false;
}

void UCoreGameplayAbility::NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
{
}

void UCoreGameplayAbility::ScriptOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
{
}

bool UCoreGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UCoreGameplayAbility::SetCanBeCanceled(bool bCanBeCanceled)
{
	Super::SetCanBeCanceled(bCanBeCanceled);
}

void UCoreGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
}

void UCoreGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UCoreGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UCoreGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UCoreGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
}

void UCoreGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
}

FGameplayEffectContextHandle UCoreGameplayAbility::MakeEffectContext(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo) const
{
	return Super::MakeEffectContext(Handle, ActorInfo);
}

void UCoreGameplayAbility::ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec,
	FGameplayAbilitySpec* AbilitySpec) const
{
	Super::ApplyAbilityTagsToGameplayEffectSpec(Spec, AbilitySpec);
}

bool UCoreGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::DoesAbilitySatisfyTagRequirements(AbilitySystemComponent, SourceTags, TargetTags,
	                                                OptionalRelevantTags);
}

void UCoreGameplayAbility::OnPawnAvatarSet()
{
}

void UCoreGameplayAbility::GetAbilitySource(FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, float& OutSourceLevel,
	const ICoreAbilitySourceInterface*& OutAbilitySource, AActor*& OutEffectCauser) const
{
}
