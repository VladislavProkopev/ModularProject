// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreInputConfig.h"

DEFINE_LOG_CATEGORY_STATIC(Log_CoreInputConfig,All,All)

UCoreInputConfig::UCoreInputConfig(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

const UInputAction* UCoreInputConfig::FindNativeInputActionForTag(const FGameplayTag InputTag, bool bLogNotFound) const
{
	for (const FCoreInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(Log_CoreInputConfig,Error,TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."),*InputTag.ToString(),*GetNameSafe(this));
	}
	return  nullptr;
}

const UInputAction* UCoreInputConfig::FindAbilityInputActionForTag(const FGameplayTag InputTag, bool bLogNotFound) const
{
	for (const FCoreInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(Log_CoreInputConfig,Error,TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."),*InputTag.ToString(),*GetNameSafe(this));
	}
	return  nullptr;
}
