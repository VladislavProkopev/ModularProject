// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreGameData.h"
#include "CoreAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CoreGameData)

UCoreGameData::UCoreGameData()
{
}

const UCoreGameData& UCoreGameData::Get()
{
	return UCoreAssetManager::Get().GetGameData();
}
