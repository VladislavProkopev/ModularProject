// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFeatureAction_AddInputContextMapping.h"

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureRegistering()
{
	Super::OnGameFeatureRegistering();
}

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
}

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureUnregistering()
{
	Super::OnGameFeatureUnregistering();
}

EDataValidationResult UGameFeatureAction_AddInputContextMapping::IsDataValid(class FDataValidationContext& Context)
{
	return Super::IsDataValid(Context);
}

void UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContexts()
{
}

void UGameFeatureAction_AddInputContextMapping::RegisterInputMappingsForGameInstance(UGameInstance* GameInstance)
{
}

void UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer)
{
}

void UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContexts()
{
}

void UGameFeatureAction_AddInputContextMapping::UnregisterInputContextMappingsForGameInstance(
	UGameInstance* GameInstance)
{
}

void UGameFeatureAction_AddInputContextMapping::UnregisterInputContextMappingsForLocalPlayer(ULocalPlayer* LocalPlayer)
{
}

void UGameFeatureAction_AddInputContextMapping::AddToWorld(const FWorldContext& WorldContext,
	const FGameFeatureStateChangeContext& ChangeContext)
{
}

void UGameFeatureAction_AddInputContextMapping::Reset(FPerContextData& ActiveData)
{
}

void UGameFeatureAction_AddInputContextMapping::HandleControllerExtension(AActor* Actor, FName EventName,
	FGameFeatureStateChangeContext ChangeContext)
{
}

void UGameFeatureAction_AddInputContextMapping::AddInputMappingForPlayer(UPlayer* Player, FPerContextData& ActiveData)
{
}

void UGameFeatureAction_AddInputContextMapping::RemoveInputMapping(APlayerController* PlayerController,
	FPerContextData& ActiveData)
{
}
