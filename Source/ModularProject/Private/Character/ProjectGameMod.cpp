// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularProject/Public/Character/ProjectGameMod.h"
#include "Engine/AssetManager.h"
#include "Tasks/Task.h"

void AProjectGameMod::PreloadPawnData(TSoftObjectPtr<UProjectPawnData> SoftPawnData)
{
	if (SoftPawnData.IsNull()) return;
	
	// Выносим тяжелую загрузку (если граф зависимостей ассета большой)
	UE::Tasks::FTask LoadTask = UE::Tasks::Launch(UE_SOURCE_LOCATION,[SoftPawnData]()
	{
		// StreamableManager асинхронно подгружает ассет в память
		UAssetManager::GetStreamableManager().LoadSynchronous(SoftPawnData.ToSoftObjectPath());
	}, UE::Tasks::ETaskPriority::BackgroundHigh);
	
	/*TODO
	Продолжаем логику спавна только когда таска завершена
	В реальном проекте мы привязываем Delegate к завершению State Machine загрузки игрока
	*/
}
