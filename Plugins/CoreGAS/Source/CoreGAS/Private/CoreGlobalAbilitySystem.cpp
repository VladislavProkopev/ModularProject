// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreGlobalAbilitySystem.h"
#include "CoreAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CoreGlobalAbilitySystem)

void FGlobalAppliedAbilityList::AddToASC(TSubclassOf<UGameplayAbility> Ability, UCoreAbilitySystemComponent* ASC)
{
	checkf(ASC,TEXT("ASC in FGlobalAppliedAbilityList::AddToASC - invalid or null"));
	checkf(Ability,TEXT("Ability in FGlobalAppliedAbilityList::AddToASC - invalid or null"));
	
	FObjectKey ASCKey(ASC);
	
	if (Handles.Contains(ASCKey))
	{
		RemoveFromASC(ASC);
	}
	
	UGameplayAbility* AbilityCDO = Ability->GetDefaultObject<UGameplayAbility>();
	FGameplayAbilitySpec AbilitySpec(AbilityCDO);
	const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
	Handles.Add(ASCKey,AbilitySpecHandle);
}

void FGlobalAppliedAbilityList::RemoveFromASC(UCoreAbilitySystemComponent* ASC)
{
	checkf(ASC,TEXT("ASC in FGlobalAppliedAbilityList::RemoveFromASC - invalid or null"));
	
	FObjectKey ASCKey(ASC);
	if (FGameplayAbilitySpecHandle* SpecHandle = Handles.Find(ASCKey))
	{
		ASC->ClearAbility(*SpecHandle);
		Handles.Remove(ASCKey);
	}
}

void FGlobalAppliedAbilityList::RemoveFromAll()
{
	for (auto& KVP : Handles)
	{
		// Пытаемся восстановить оригинальный указатель из FObjectKey.
		// Если объект был уничтожен в обход UnregisterASC, это безопасно вернет nullptr.
		if (UCoreAbilitySystemComponent* ASC = Cast<UCoreAbilitySystemComponent>(KVP.Key.ResolveObjectPtr()))
		{
			ASC->ClearAbility(KVP.Value);
		}
	}
	Handles.Empty();
}

void FGlobalAppliedEffectList::AddToASC(TSubclassOf<UGameplayEffect> Effect, UCoreAbilitySystemComponent* ASC)
{
	if (FActiveGameplayEffectHandle* Handle = Handles.Find(ASC))
	{
		RemoveFromASC(ASC);
	}
	
	const UGameplayEffect* EffectCDO = Effect->GetDefaultObject<UGameplayEffect>();
	const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(EffectCDO,1,ASC->MakeEffectContext());
	Handles.Add(ASC,GameplayEffectHandle);
}

void FGlobalAppliedEffectList::RemoveFromASC(UCoreAbilitySystemComponent* ASC)
{
	if (FActiveGameplayEffectHandle* Handle = Handles.Find(ASC))
	{
		ASC->RemoveActiveGameplayEffect(*Handle);
		Handles.Remove(ASC);
	}
}

void FGlobalAppliedEffectList::RemoveFromAll()
{
	for (auto& KVP : Handles)
	{
		// Пытаемся восстановить оригинальный указатель из FObjectKey.
		// Если объект был уничтожен в обход UnregisterASC, это безопасно вернет nullptr.
		if (UCoreAbilitySystemComponent* ASC = Cast<UCoreAbilitySystemComponent>(KVP.Key.ResolveObjectPtr()))
		{
			ASC->RemoveActiveGameplayEffect(KVP.Value);
		}
	}
	Handles.Empty();
}

void UCoreGlobalAbilitySystem::ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability)
{
	if (Ability && !AppliedAbilities.Contains(Ability))
	{
		FGlobalAppliedAbilityList& Entry = AppliedAbilities.Add(Ability);
		
		for (const TWeakObjectPtr<UCoreAbilitySystemComponent>& WeakASC : RegisteredASCs)
		{
			if (UCoreAbilitySystemComponent* ASC = WeakASC.Get())
			{
				Entry.AddToASC(Ability,ASC);
			}
		}
	}
}

void UCoreGlobalAbilitySystem::ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect)
{
	if ((Effect.Get() != nullptr)&&(!AppliedEffects.Contains(Effect)))
	{
		FGlobalAppliedEffectList& Entry = AppliedEffects.Add(Effect);
		for (const TWeakObjectPtr<UCoreAbilitySystemComponent>& WeakASC : RegisteredASCs)
		{
			if (UCoreAbilitySystemComponent* ASC = WeakASC.Get())
			{
				Entry.AddToASC(Effect,ASC);
			}
		}
	}
}

void UCoreGlobalAbilitySystem::RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability)
{
	if ((Ability.Get() != nullptr)&& AppliedAbilities.Contains(Ability))
	{
		FGlobalAppliedAbilityList& Entry = AppliedAbilities[Ability];
		Entry.RemoveFromAll();
		AppliedAbilities.Remove(Ability);
	}
}

void UCoreGlobalAbilitySystem::RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect)
{
	if ((Effect.Get() != nullptr)&& AppliedEffects.Contains(Effect))
	{
		FGlobalAppliedEffectList& Entry = AppliedEffects[Effect];
		Entry.RemoveFromAll();
		AppliedEffects.Remove(Effect);
	}
}

void UCoreGlobalAbilitySystem::RegisterASC(UCoreAbilitySystemComponent* ASC)
{
	checkf(ASC,TEXT("ASC in UCoreGlobalAbilitySystem::RegisterASC - invalid or null"));
	
	// Предполагается, что компонент вызывает Register 1 раз на BeginPlay.
	RegisteredASCs.Add(ASC);
	
	for (auto& Entry : AppliedAbilities)
	{
		Entry.Value.AddToASC(Entry.Key,ASC);
	}
	for (auto& Entry : AppliedEffects)
	{
		Entry.Value.AddToASC(Entry.Key,ASC);
	}
	
	/* Возможно использование AddUnique - дороже но не нужно контролировать разовую инициализацию
	RegisteredASCs.AddUnique(ASC);
	*/
}

void UCoreGlobalAbilitySystem::UnregisterASC(UCoreAbilitySystemComponent* ASC)
{
	checkf(ASC,TEXT("ASC in UCoreGlobalAbilitySystem::UnregisterASC - invalid or null"));
	
	for (auto& Entry : AppliedAbilities)
	{
		Entry.Value.RemoveFromASC(ASC);
	}
	for (auto& Entry : AppliedEffects)
	{
		Entry.Value.RemoveFromASC(ASC);
	}
	// Swap-удаление. Это O(1) вместо O(N), так как не сдвигает весь массив,
	// а просто меняет удаляемый элемент с последним. Порядок в RegisteredASCs нам не важен.
	RegisteredASCs.RemoveSwap(ASC);
}
