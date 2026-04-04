// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbility.h"
#include "CoreGameplayAbility.generated.h"

#define GAS_API COREGAS_API

struct FGameplayAbilityActivationInfo;
struct FGameplayAbilitySpec;
struct FGameplayAbilitySpecHandle;

class AActor;
class AController;
/* Подумать над этими классами потому что плагин ГАС лежит не в корне как в проекте Лира, а будет отдельным плагином
class ACoreCharacter;
class ACoreController
*/
class ACharacter;
class APlayerController;
class FText;
class ICoreAbilitySourceInterface;
class UAnimMontage;
class UCoreAbilityCost;
class UCoreAbilitySystemComponent;
class UObject;
struct FFrame;
struct FGameplayAbilityActorInfo;
struct FGameplayEffectSpec;
struct FGameplayEventData;

/**
 * ECoreAbilityActivationPolicy
 *
 *	Defines how an ability is meant to activate.
 */
UENUM(BlueprintType)
enum class ECoreAbilityActivationPolity : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,
	// Continually try to activate the ability while the input is active.
	WhileInputActive,
	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};

/**
 * ECoreAbilityActivationGroup
 *
 *	Defines how an ability activates in relation to other abilities.
 */

UENUM(BlueprintType)
enum class ECoreAbilityActivationGroup : uint8
{
	// Ability runs independently of all other abilities.
	Independent,
	// Ability is canceled and replaced by other exclusive abilities.
	Exclusive_Replaceable,
	// Ability blocks all other exclusive abilities from activating.
	Exclusive_Blocking,
	
	MAX UMETA(Hidden)
};

/** Failure reason that can be used to play an animation montage when a failure occurs */
USTRUCT(BlueprintType)
struct FCoreAbilityMontageFaltureMessage
{
	GENERATED_BODY()
	
public:
	// Player controller that failed to activate the ability, if the AbilitySystemComponent was player owned
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;
	// Avatar actor that failed to activate the ability
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> AvatarActor = nullptr;
	// All the reasons why this ability has failed
	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer FailtureTags;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAnimMontage> FailtureMontage = nullptr;
};

/**
* ULyraGameplayAbility
 *
 *	The base gameplay ability class used by this project.
 */
UCLASS(MinimalAPI,Abstract,HideCategories=(Input),Meta = (ShortTooltip = "The base gameplay ability class used"))
class UCoreGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	friend class UCoreAbilitySystemComponent;
	
public:
	GAS_API UCoreGameplayAbility(const FObjectInitializer& OI);
	
	UFUNCTION(BlueprintCallable,Category="Core|Ability")
	GAS_API UCoreAbilitySystemComponent* GetCoreAbilitySystemComponentFromActorInfo() const;
	
	//-----------------------------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable,Category="Core|Ability")
	/*TODO
	*В лире тут используется жесткая связь так как гас лежит в проекте а я планирую выносить в плагин
	*UE_API ALyraPlayerController* GetLyraPlayerControllerFromActorInfo() const;
	*Подумать после над реализацией
	*/
	GAS_API APlayerController* GetPlayerControllerFromActorInfo() const;
	//-----------------------------------------------------------------------------------------------------
	
	UFUNCTION(BlueprintCallable,Category="Core|Ability")
	GAS_API AController* GetControllerFromActorInfo() const;
	
	UFUNCTION(BlueprintCallable,Category="Core|Ability")
	GAS_API ACharacter* GetCharacterFromActorInfo() const;
	
	/*TODO Пересмотреть структуру на использование этих методов в переопределенном классе CoreGameplayAbility 
	 *непосредственно в модуле проекта
	 *-----------------------------------------------------------------------------------------------------
	 *UFUNCTION(BlueprintCallable,Category="Core|Ability")
	 *
	 *В лире тут используется жесткая связь так как гас лежит в проекте а я планирую выносить в плагин
	 *UE_API ALyraCharacter* GetLyraCharacterFromActorInfo() const;
	 *Подумать после над реализацией
	 *
	 *GAS_API ACoreCharacter* GetCoreCharacterFromActorInfo() const;
	 *-----------------------------------------------------------------------------------------------------
	 *
	 *TODO Create UCoreHeroComponent
	 *UFUNCTION(BlueprintCallable,Category="Core|Ability")
	 *GAS_API UCoreHeroComponent* GetHeroComponentFromActorInfo() const;
	 */
	ECoreAbilityActivationPolity GetActivationPolity() const {return ActivationPolity;}
	ECoreAbilityActivationGroup GetActivationGroup() const {return ActivationGroup;}
	
	GAS_API void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;
	// Returns true if the requested activation group is a valid transition.
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category="Core|Ability",Meta = (ExpandBoolAsExecs = "ReturnValue"))
	GAS_API bool CanChangeActivationGroup(ECoreAbilityActivationGroup NewGroup) const;
	// Tries to change the activation group.  Returns true if it successfully changed.
	UFUNCTION(BlueprintCallable,BlueprintPure = false, Category="Core|Ability",Meta = (ExpandBoolAsExecs = "ReturnValue"))
	GAS_API bool ChangeActivationGroup(ECoreAbilityActivationGroup NewGroup);
	
	/*TODO Create UCoreCameraMode
	 *Перенести CameraMode в отдельный плагин, дабы не раздувать ГАС классы
	 *UFUNCTION(BlueprintCallable,Category="Core|Ability")
	 *GAS_API void SetCameraMode(TSubclassOf<UCoreCameraMode> CameraMode);
	 *UFUNCTION(BlueprintCallable,Category="Core|Ability")
	 *GAS_API ClearCameraMode();
	 * 
	 */
	void OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason)const
	{
		NativeOnAbilityFailedToActivate(FailedReason);
		ScriptOnAbilityFailedToActivate(FailedReason);
	}
	
protected:
	// Called when the ability fails to activate
	GAS_API virtual void NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;
	
	// Called when the ability fails to activate
	GAS_API void ScriptOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;
	
	//~ Begin UGameplayAbilityInterface
	GAS_API virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	GAS_API virtual void SetCanBeCanceled(bool bCanBeCanceled) override;
	GAS_API virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	GAS_API virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	GAS_API virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	GAS_API virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	GAS_API virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	GAS_API virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	GAS_API virtual FGameplayEffectContextHandle MakeEffectContext(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const override;
	GAS_API virtual void ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec, FGameplayAbilitySpec* AbilitySpec) const override;
	GAS_API virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	//~ End UGameplayAbilityInterface
	
	GAS_API virtual void OnPawnAvatarSet();
	
	GAS_API virtual void GetAbilitySource(FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,float& OutSourceLevel, const ICoreAbilitySourceInterface*& OutAbilitySource, AActor*& OutEffectCauser) const;
	/** Called when this ability is granted to the ability system component. */
	UFUNCTION(BlueprintImplementableEvent,Category=Ability,DisplayName="OnAbilityAdded")
	GAS_API void K2_OnAbilityAdded();
	/** Called when this ability is removed from the ability system component. */
	UFUNCTION(BlueprintImplementableEvent,Category=Ability,DisplayName="OnAbilityRemoved")
	GAS_API void K2_OnAbilityRemoved();
	/** Called when the ability system is initialized with a pawn avatar. */
	UFUNCTION(BlueprintImplementableEvent,Category=Ability,DisplayName="OnPawnAvatarSet")
	GAS_API void K2_OnPawnAvatarSet();
	
	
	// Defines how this ability is meant to activate.
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Core|Ability Activation")
	ECoreAbilityActivationPolity ActivationPolity;
	
	// Defines the relationship between this ability activating and other abilities activating.
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Core|Ability Activation")
	ECoreAbilityActivationGroup ActivationGroup;
	
	// Additional costs that must be paid to activate this ability
	UPROPERTY(EditDefaultsOnly,Instanced,Category=Costs)
	TArray<TObjectPtr<UCoreAbilityCost>> AdditionalCosts;
	
	// Map of failure tags to simple error messages
	UPROPERTY(EditDefaultsOnly,Category=Advanced)
	TMap<FGameplayTag,FText> FailtureTagToUserFacingMessages;
	
	// Map of failure tags to anim montages that should be played with them
	UPROPERTY(EditDefaultsOnly,Category=Advanced)
	TMap<FGameplayTag,TObjectPtr<UAnimMontage>> FailtureAnimMontage;
	
	// If true, extra information should be logged when this ability is canceled. This is temporary, used for tracking a bug.
	UPROPERTY(EditDefaultsOnly,Category=Advanced)
	bool bLogCancelation;
	
	// Current camera mode set by the ability.
	/*TODO UCoreCameraMode
	*TSubclassOf<UCoreCameraMode> ActiveCameraMode;
	*Перенести логику камеры в отдельный плагин, дабы не раздувать ГАС и не делать его ГОД обьектом
	*/
};

#undef GAS_API