// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreLoadingProcessTask.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "CoreLoadingScreenManager.h"
#include "UObject/ScriptInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CoreLoadingProcessTask)

/*static*/UCoreLoadingProcessTask* UCoreLoadingProcessTask::CreateCoreLoadingProcessTask(UObject* WorldContextObject,
	const FString& ShowLoadingScreenReason)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject,EGetWorldErrorMode::LogAndReturnNull);
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	UCoreLoadingScreenManager* LoadingScreenManager = GameInstance ? GameInstance->GetSubsystem<UCoreLoadingScreenManager>() : nullptr;

	if (LoadingScreenManager)
	{
		UCoreLoadingProcessTask* NewLoadingTask = NewObject<UCoreLoadingProcessTask>(LoadingScreenManager);
		NewLoadingTask->SetShowLoadingScreenReason(ShowLoadingScreenReason);
		
		LoadingScreenManager->RegisterLoadingProcessor(NewLoadingTask);
		
		return NewLoadingTask;
	}
	return nullptr;
}

void UCoreLoadingProcessTask::Unregister()
{
	UCoreLoadingScreenManager* LoadingScreenManager = Cast<UCoreLoadingScreenManager>(GetOuter());
	LoadingScreenManager->UnregisterLoadingProcessor(this);
}

void UCoreLoadingProcessTask::SetShowLoadingScreenReason(const FString& InReason)
{
	Reason = InReason;
}

bool UCoreLoadingProcessTask::ShouldShowLoadingScreen(FString& OutReason)
{
	OutReason = Reason;
	return true;
}
