// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularProject/Public/Character/ProjectGameMod.h"
#include "Engine/AssetManager.h"
#include "ModularProject/Character/ProjectCharacter.h"
#include "ModularProject/Character/ProjectPawnData.h"
#include "ModularProject/Character/ProjectPawnExtensionComponent.h"
#include "Tasks/Task.h"

void AProjectGameMod::PreloadPawnData(TSoftObjectPtr<UProjectPawnData> SoftPawnData)
{
	if (SoftPawnData.IsNull()) return;
	
	FSoftObjectPath AssetPath = SoftPawnData.ToSoftObjectPath();
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	
	PawnDataLoadHandle = StreamableManager.RequestAsyncLoad(AssetPath,FStreamableDelegate::CreateUObject(this, &AProjectGameMod::OnPawnDataLoaded));
}

void AProjectGameMod::OnPawnDataLoaded()
{
	if (PawnDataLoadHandle && PawnDataLoadHandle->HasLoadCompleted())
	{
		UProjectPawnData* LoadedPawnData = Cast<UProjectPawnData>(PawnDataLoadHandle->GetLoadedAsset());
		//TODO Create SpawnPoints Manager Subsystem
		//FindPlayerStart()
		AProjectCharacter* SpawnedActor = GetWorld()->SpawnActor<AProjectCharacter>(LoadedPawnData->PawnClass);
		SpawnedActor->GetPawnExtensionComponent()->SetPawnData(LoadedPawnData);
		
		PawnDataLoadHandle.Reset();
	}
}
