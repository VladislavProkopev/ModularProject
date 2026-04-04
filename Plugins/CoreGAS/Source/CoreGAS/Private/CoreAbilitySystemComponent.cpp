#include "CoreAbilitySystemComponent.h"
#include "CoreGameplayAbility.h"
#include "CoreAbilityTagRelationshipMapping.h"
#include "CoreGlobalAbilitySystem.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"


UCoreAbilitySystemComponent::UCoreAbilitySystemComponent(const FObjectInitializer& OI)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
	
	InputHeldSpecHandles.Reset();
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UCoreAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (UCoreGlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<UCoreGlobalAbilitySystem>(GetWorld()))
	{
		GlobalAbilitySystem->UnregisterASC(this);
	}
}

void UCoreAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
}

void UCoreAbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc,
	bool bReplicateCancelAbility)
{
}

void UCoreAbilitySystemComponent::CancelInputActivatedAbilities(bool bReplicateCancelAbility)
{
}

void UCoreAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
}

void UCoreAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
}

void UCoreAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
}

void UCoreAbilitySystemComponent::ClearAbilityInput()
{
}

bool UCoreAbilitySystemComponent::IsActivationGroupBlocked(ECoreAbilityActivationGroup Group) const
{
	//Заглушка
	return false;
}

void UCoreAbilitySystemComponent::AddAbilityToActivationGroup(ECoreAbilityActivationGroup Group,
	UCoreGameplayAbility CoreAbility)
{
}

void UCoreAbilitySystemComponent::RemoveAbilityFromActivationGroup(ECoreAbilityActivationGroup Group,
	UCoreGameplayAbility CoreAbility)
{
}

void UCoreAbilitySystemComponent::CancelActivationGroupAbilities(ECoreAbilityActivationGroup Group,
	UCoreGameplayAbility CoreAbility, bool bReplicateCancelAbility)
{
}

void UCoreAbilitySystemComponent::AddDynamicTagGameplayEffect(const FGameplayTag& Tag)
{
}

void UCoreAbilitySystemComponent::RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag)
{
}

void UCoreAbilitySystemComponent::GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle,
	FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle)
{
}

void UCoreAbilitySystemComponent::SetTargetRelationshipMapping(UCoreAbilityTagRelationshipMapping* NewMapping)
{
}

void UCoreAbilitySystemComponent::GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags,
	FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const
{
}

void UCoreAbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
}

void UCoreAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);
}

void UCoreAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);
}

void UCoreAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle,
	UGameplayAbility* Ability)
{
	Super::NotifyAbilityActivated(Handle, Ability);
}

void UCoreAbilitySystemComponent::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle,
	UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	Super::NotifyAbilityFailed(Handle, Ability, FailureReason);
}

void UCoreAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability,
	bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);
}

void UCoreAbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags,
	UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags,
	bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags)
{
	Super::ApplyAbilityBlockAndCancelTags(AbilityTags, RequestingAbility, bEnableBlockTags, BlockTags,
	                                      bExecuteCancelTags, CancelTags);
}

void UCoreAbilitySystemComponent::HandleChangeAbilityCanBeCanceled(const FGameplayTagContainer& AbilityTags,
	UGameplayAbility* RequestingAbility, bool bCanBeCanceled)
{
	Super::HandleChangeAbilityCanBeCanceled(AbilityTags, RequestingAbility, bCanBeCanceled);
}

void UCoreAbilitySystemComponent::HandleAbilityFailed(const UGameplayAbility* Ability,
	const FGameplayTagContainer& FailureReason)
{
}

void UCoreAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCoreAbilitySystemComponent::ClientNotifyAbilityFailed_Implementation(const UGameplayAbility* Ability,
	const FGameplayTagContainer& FailureReason)
{
}


