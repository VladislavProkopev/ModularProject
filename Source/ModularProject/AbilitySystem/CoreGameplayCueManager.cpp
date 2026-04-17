// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreGameplayCueManager.h"
#include "Engine/AssetManager.h"
#include "GameplayCueSet.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTagsManager.h"
#include "UObject/UObjectThreadContext.h"
#include "Async/Async.h"
#include "ModularProject/System/CoreAssetManager.h"
#include "ModularProject/System/CoreGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CoreGameplayCueManager)

DEFINE_LOG_CATEGORY_STATIC(Log_CoreGameplayCueManager, All, All);

enum class ECoreEditorLoadMode
{
	//Load all cues upfront; longer loading speed in the editor but short PIE times and effects never fail to play
	LoadUpfront,
	//Outside of editor: Async loads as cue tag are registered
	//In editor: Async loads when cues are invoked
	//Note: This can cause some 'why didn't I see the effect for X' issues in PIE and is good for iteration speed but otherwise bad for designers
	PreloadAsCuesAreReferenced_GameOnly,
	//Async loads as cue tag are registered
	PreloadAsCuesAreReferenced
};

namespace CoreGameplayCueManagerCVars
{
	static FAutoConsoleCommand CVarDumpGameplayCues(
		TEXT("ModularProject.DumpGameplayCues"),
		TEXT("Shows all assets that were loaded via CoreGameplayCueManager and are currently in memory"),
		FConsoleCommandWithArgsDelegate::CreateStatic(UCoreGameplayCueManager::DumpGameplayCues));
	
	static ECoreEditorLoadMode LoadMode = ECoreEditorLoadMode::LoadUpfront;
}

const bool bPreloadEventInEditor = true;

//////////////////////////////////////////////////////////////////////////////////////////////////////////

struct FGameplayCueTagThreadSynchronizeGraphTask : public FAsyncGraphTaskBase
{
	TFunction<void()> TheTask;
	FGameplayCueTagThreadSynchronizeGraphTask(TFunction<void()>&& Task) : TheTask(MoveTemp(Task)) {}
	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent) {TheTask();}
	ENamedThreads::Type GetDesiredThread() {return ENamedThreads::GameThread;}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

UCoreGameplayCueManager::UCoreGameplayCueManager(const FObjectInitializer& ObjectInitializer)
{
}

UCoreGameplayCueManager* UCoreGameplayCueManager::Get()
{
	return Cast<UCoreGameplayCueManager>(UAbilitySystemGlobals::Get().GetGameplayCueManager());
}

void UCoreGameplayCueManager::OnCreated()
{
	Super::OnCreated();
	
	UpdateDelayLoadDelegateListeners();
}

bool UCoreGameplayCueManager::ShouldAsyncLoadRuntimeObjectLibraries() const
{
	switch (CoreGameplayCueManagerCVars::LoadMode)
	{
	case ECoreEditorLoadMode::LoadUpfront:
		return true;
	case ECoreEditorLoadMode::PreloadAsCuesAreReferenced_GameOnly:
#if WITH_EDITOR
		if (GIsEditor)
		{
			return false;
		}
#endif
		break;
	case ECoreEditorLoadMode::PreloadAsCuesAreReferenced:
		break;
	}
	return !ShouldDelayLoadGameplayCues();
}

bool UCoreGameplayCueManager::ShouldAsyncLoadMissingGameplayCues() const
{
	return true;
}

bool UCoreGameplayCueManager::ShouldSyncLoadMissingGameplayCues() const
{
	return false;
}

void UCoreGameplayCueManager::DumpGameplayCues(const TArray<FString>& Args)
{
	UCoreGameplayCueManager* GCM = Cast<UCoreGameplayCueManager>(UAbilitySystemGlobals::Get().GetGameplayCueManager());
	if (!GCM)
	{
		UE_LOG(Log_CoreGameplayCueManager,Error,TEXT("DumpGameplayCues failed. No UCoreGameplayCueManager found"));
		return;
	}
	
	const bool bIncludeRefs = Args.Contains(TEXT("Refs"));
	
	UE_LOG(Log_CoreGameplayCueManager,Log,TEXT("============= Dumping Always Loaded Gameplay Cue Notifies ==============="));
	for (UClass* CueClass : GCM->AlwaysLoadedCues)
	{
		UE_LOG(Log_CoreGameplayCueManager, Log, TEXT("   %s"),*GetPathNameSafe(CueClass));
	}
	
	UE_LOG(Log_CoreGameplayCueManager,Log,TEXT("============= Dumping Preloaded Gameplay Cue Notifies ===================="));
	for (UClass* CueClass : GCM->PreloadedCues)
	{
		TSet<FObjectKey>* ReferencerSet = GCM->PreLoadedCueReferencers.Find(CueClass);
		int32 NumRefs = ReferencerSet ? ReferencerSet->Num() : 0;
		UE_LOG(Log_CoreGameplayCueManager,Log,TEXT("  %s (%d refs)"),*GetPathNameSafe(CueClass),NumRefs);
		if (bIncludeRefs && ReferencerSet)
		{
			for (const FObjectKey& Ref : *ReferencerSet)
			{
				UObject* RefObject = Ref.ResolveObjectPtr();
				UE_LOG(Log_CoreGameplayCueManager,Log, TEXT("   ^- %s"),*GetPathNameSafe(RefObject));
			}
		}
	}
	
	UE_LOG(Log_CoreGameplayCueManager,Log,TEXT("=============== Dumping Gameplay Cue Notifies loaded on demand ==============="));
	int32 NumMissingCueLoaded = 0;
	if (GCM->RuntimeGameplayCueObjectLibrary.CueSet)
	{
		for (const FGameplayCueNotifyData& CueData: GCM->RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueData)
		{
			if (CueData.LoadedGameplayCueClass && !GCM->AlwaysLoadedCues.Contains(CueData.LoadedGameplayCueClass) && !GCM->PreloadedCues.Contains(CueData.LoadedGameplayCueClass))
			{
				NumMissingCueLoaded++;
				UE_LOG(Log_CoreGameplayCueManager,Log,TEXT("   %s"),*CueData.LoadedGameplayCueClass->GetPathName());
			}
		}
	}
	
	UE_LOG(Log_CoreGameplayCueManager,Log,TEXT("=============== Gameplay Cue Notify summary ==============="));
	UE_LOG(Log_CoreGameplayCueManager,Log,TEXT(" ... %d cues in always loaded list"),GCM->AlwaysLoadedCues.Num());
	UE_LOG(Log_CoreGameplayCueManager,Log,TEXT(" ... %d cues in preloaded list"),GCM->PreloadedCues.Num());
	UE_LOG(Log_CoreGameplayCueManager,Log,TEXT(" ... %d cues loaded demand"),NumMissingCueLoaded);
	UE_LOG(Log_CoreGameplayCueManager,Log,TEXT(" ... %d cues in total"),GCM->AlwaysLoadedCues.Num() + GCM->PreloadedCues.Num() + NumMissingCueLoaded);
}

void UCoreGameplayCueManager::LoadAlwaysLoadedCues()
{
	if (ShouldDelayLoadGameplayCues())
	{
		const UCoreGameData& GameData = UCoreAssetManager::Get().GetGameData();

		for (const FGameplayTag& CueTag : GameData.AlwaysLoadedGameplayCues)
		{
			if (CueTag.IsValid())
			{
				ProcessTagToPreload(CueTag, nullptr);
			}
			else
			{
				UE_LOG(Log_CoreGameplayCueManager,Error,TEXT("CueTag is Invalid in UCoreGameData GameData.AlwaysLoadedGameplayCues"));
			}
		}
		
		/*LyraStyle snippet
		 *UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();
		
		//TODO Try to collect these by filtering GameplayCue. Tags out of native gameplay tags?
		TArray<FName> AdditionalAlwaysLoadedCueTags;
		
		for (const FName& CueTagName : AdditionalAlwaysLoadedCueTags)
		{
			FGameplayTag CueTag = TagsManager.RequestGameplayTag(CueTagName,/* ErrorIfNotFound #1# false);
			if (CueTag.IsValid())
			{
				ProcessTagToPreload(CueTag,nullptr);
			}
			else
			{
				UE_LOG(Log_CoreGameplayCueManager,Warning,TEXT("UCoreGameplayCueManager::AdditionalAlwaysLoadedCueTags contains invalid tag %s"),*CueTagName.ToString());
			}
		}*/
	}
}



void UCoreGameplayCueManager::OnGameplayTagLoaded(const FGameplayTag& Tag)
{
	FScopeLock ScopeLock(&LoadedGameplayTagsToProcessCS);
	bool bStartTask = LoadedGameplayTagsToProcess.Num() == 0;
	FUObjectSerializeContext* LoadContext = FUObjectThreadContext::Get().GetSerializeContext();
	UObject* OwningObject = LoadContext ? LoadContext->SerializedObject : nullptr;
	LoadedGameplayTagsToProcess.Emplace(Tag, OwningObject);
	if (bStartTask)
	{
		TGraphTask<FGameplayCueTagThreadSynchronizeGraphTask>::CreateTask().ConstructAndDispatchWhenReady([]()
		{
			if (GIsRunning)
			{
				if (UCoreGameplayCueManager* StrongThis = Get())
				{
					//If we are garbage collecting we cannot call StaticFindObject (or a few other static uobject functions), so we'll just wait until the GC is over and process the tags then
					if (IsGarbageCollecting())
					{
						StrongThis->bProcessLoadedTagsAfterGC = true;
					}
					else
					{
						StrongThis->ProcessLoadedTags();
					}
				}
			}
		});
	}
}

void UCoreGameplayCueManager::HandlePostGarbageCollect()
{
	if (bProcessLoadedTagsAfterGC)
	{
		ProcessLoadedTags();
	}
	bProcessLoadedTagsAfterGC = false;
}

void UCoreGameplayCueManager::ProcessLoadedTags()
{
	TArray<FLoadedGameplayTagToProcessData> TaskLoadedGameplayTagsToProcess;
	{
		//Lock LoadedGameplayTagsToProcess just long enough to make a copy and clear
		FScopeLock TaskScopeLock(&LoadedGameplayTagsToProcessCS);
		TaskLoadedGameplayTagsToProcess = LoadedGameplayTagsToProcess;
		LoadedGameplayTagsToProcess.Empty();
	}
	
	//This might return during shutdown, and we don't want to proceed if that is came
	if (GIsRunning)
	{
		if (RuntimeGameplayCueObjectLibrary.CueSet)
		{
			for (const FLoadedGameplayTagToProcessData& LoadedTagData : TaskLoadedGameplayTagsToProcess)
			{
				if (RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueDataMap.Contains(LoadedTagData.Tag))
				{
					if (!LoadedTagData.WeakOwner.IsStale())
					{
						ProcessTagToPreload(LoadedTagData.Tag,LoadedTagData.WeakOwner.Get());
					}
				}
			}
		}
		else
		{
			UE_LOG(Log_CoreGameplayCueManager,Warning,TEXT("UCoreGameplayCueManager::OnGameplayTagLoaded processes loaded tag(s) but RuntimeGameplayCueObjectLibrary.CueSet was null. Skipping processing"));
		}
	}
}

void UCoreGameplayCueManager::ProcessTagToPreload(const FGameplayTag& Tag, UObject* OwningObject)
{
	//TODO Разобраться с целесобразностью этого switch который по сути не делает ничего
	switch (CoreGameplayCueManagerCVars::LoadMode)
	{
	case ECoreEditorLoadMode::LoadUpfront:
		return;
	case ECoreEditorLoadMode::PreloadAsCuesAreReferenced_GameOnly:
#if WITH_EDITOR
		if (GIsEditor)
		{
			return;
		}
#endif
		break;
	case ECoreEditorLoadMode::PreloadAsCuesAreReferenced:
		break;
		
	}
	
	check(RuntimeGameplayCueObjectLibrary.CueSet);
	
	int32* DataIdx = RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueDataMap.Find(Tag);
	if (DataIdx && RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueData.IsValidIndex(*DataIdx))
	{
		const FGameplayCueNotifyData& CueData = RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueData[*DataIdx];
		
		UClass* LoadedGameplayCueClass = FindObject<UClass>(nullptr,*CueData.GameplayCueNotifyObj.ToString());
		if (LoadedGameplayCueClass)
		{
			RegisterPreLoadedCue(LoadedGameplayCueClass,OwningObject);
		}
		else
		{
			bool bAlwaysLoadedCue = OwningObject == nullptr;
			TWeakObjectPtr<UObject> WeakOwner = OwningObject;
			StreamableManager.RequestAsyncLoad(CueData.GameplayCueNotifyObj,FStreamableDelegate::CreateUObject(this,&ThisClass::OnPreloadCueComplete,CueData.GameplayCueNotifyObj,WeakOwner,bAlwaysLoadedCue),FStreamableManager::DefaultAsyncLoadPriority,false,false,TEXT("GameplayCueManager"));
		}
	}
	
}

void UCoreGameplayCueManager::OnPreloadCueComplete(FSoftObjectPath Path, TWeakObjectPtr<UObject> OwningObject,
	bool bAlwaysLoadedCue)
{
	if (bAlwaysLoadedCue || OwningObject.IsValid())
	{
		if (UClass* LoadedGameplayCueClass = Cast<UClass>(Path.ResolveObject()))
		{
			RegisterPreLoadedCue(LoadedGameplayCueClass,OwningObject.Get());
		}
	}
}

void UCoreGameplayCueManager::RegisterPreLoadedCue(UClass* LoadedGameplayCueClass, UObject* OwningObject)
{
	check(LoadedGameplayCueClass);
	
	const bool bAlwaysLoadedCue = OwningObject == nullptr;
	if (bAlwaysLoadedCue)
	{
		AlwaysLoadedCues.Add(LoadedGameplayCueClass);
		PreloadedCues.Remove(LoadedGameplayCueClass);
		PreLoadedCueReferencers.Remove(LoadedGameplayCueClass);
	}
	else if ((OwningObject != LoadedGameplayCueClass) && (OwningObject != LoadedGameplayCueClass->GetDefaultObject()) && !AlwaysLoadedCues.Contains(LoadedGameplayCueClass))
	{
		PreloadedCues.Add(LoadedGameplayCueClass);
		TSet<FObjectKey>& ReferencerSet = PreLoadedCueReferencers.FindOrAdd(LoadedGameplayCueClass);
		ReferencerSet.Add(OwningObject);
	}
}

void UCoreGameplayCueManager::HandlePostLoadMap(UWorld* NewWorld)
{
	if (RuntimeGameplayCueObjectLibrary.CueSet)
	{
		for (UClass* CueClass : AlwaysLoadedCues)
		{
			RuntimeGameplayCueObjectLibrary.CueSet->RemoveLoadedClass(CueClass);
		}
		
		for (UClass* CueClass : PreloadedCues)
		{
			RuntimeGameplayCueObjectLibrary.CueSet->RemoveLoadedClass(CueClass);
		}
	}
	
	for (auto CueIt = PreloadedCues.CreateIterator(); CueIt; ++CueIt)
	{
		TSet<FObjectKey>& ReferenserSet = PreLoadedCueReferencers.FindChecked(*CueIt);
		for (auto RefIt = ReferenserSet.CreateIterator(); RefIt; ++RefIt)
		{
			if (!RefIt->ResolveObjectPtr())
			{
				RefIt.RemoveCurrent();
			}
		}
		if (ReferenserSet.Num() == 0)
		{
			PreLoadedCueReferencers.Remove(*CueIt);
			CueIt.RemoveCurrent();
		}
	}
}

void UCoreGameplayCueManager::UpdateDelayLoadDelegateListeners()
{
	UGameplayTagsManager::Get().OnGameplayTagLoadedDelegate.RemoveAll(this);
	FCoreUObjectDelegates::GetPostGarbageCollect().RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	switch (CoreGameplayCueManagerCVars::LoadMode)
	{
	case ECoreEditorLoadMode::LoadUpfront:
		return;
	case ECoreEditorLoadMode::PreloadAsCuesAreReferenced_GameOnly:
#if WITH_EDITOR
		if (GIsEditor)
		{
			return;
		}
#endif
		break;
	case ECoreEditorLoadMode::PreloadAsCuesAreReferenced:
		break;
	}
	
	UGameplayTagsManager::Get().OnGameplayTagLoadedDelegate.AddUObject(this,&ThisClass::OnGameplayTagLoaded);
	FCoreUObjectDelegates::GetPostGarbageCollect().AddUObject(this,&ThisClass::HandlePostGarbageCollect);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this,&ThisClass::HandlePostLoadMap);
}

bool UCoreGameplayCueManager::ShouldDelayLoadGameplayCues() const
{
	const bool bClientDelayLoadGameplayCues = true;
	return !IsRunningDedicatedServer() && bClientDelayLoadGameplayCues;
}

const FPrimaryAssetType UFortAssetManager_GameplayCueRefsType = TEXT("GameplayCueRefs");
const FName UFortAssetManager_GameplayCueRefsName = TEXT("GameplayCueReferences");
const FName UFortAssetManager_LoadStateClient = TEXT("Client");

void UCoreGameplayCueManager::RefreshGameplayCuePrimaryAsset()
{
	TArray<FSoftObjectPath> CuePaths;
	UGameplayCueSet* RuntimeGameplayCueSet = GetRuntimeCueSet();
	if (RuntimeGameplayCueSet)
	{
		RuntimeGameplayCueSet->GetSoftObjectPaths(CuePaths);
	}
	
	FAssetBundleData BundleData;
	BundleData.AddBundleAssetsTruncated(UFortAssetManager_LoadStateClient,CuePaths);
	
	FPrimaryAssetId PrimaryAssetId = FPrimaryAssetId(UFortAssetManager_GameplayCueRefsType,UFortAssetManager_GameplayCueRefsName);
	UAssetManager::Get().AddDynamicAsset(PrimaryAssetId,FSoftObjectPath(),BundleData);
}