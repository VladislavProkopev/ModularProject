// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreAssetManager.h"
#include "CoreGameplayTags.h"
#include "CoreGameData.h"
#include "AbilitySystemGlobals.h"
#include "ModularProject/Character/ProjectPawnData.h"
#include "Misc/App.h"
#include "Stats/StatsMisc.h"
#include "Engine/Engine.h"
//TODO CoreGameplayCueManager
//#include "AbilitySystem/CoreAbilitySystemCueManager.h'
#include "Misc/ScopedSlowTask.h"
#include "CoreAssetManagerStartupJob.h"
#include "ModularProject/AbilitySystem/CoreGameplayCueManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CoreAssetManager)

DEFINE_LOG_CATEGORY_STATIC(Log_AssetManager, All, All);

const FName FCoreBundles::Equipped("Equipped");

////////////////////////////////////////////////////////////////////

static FAutoConsoleCommand CVarDumpLoadedAssets(
	TEXT("Core.DumpLoadedAssets"),
	TEXT("Shows all assets that were loaded via the asset manager and are currently in memory."),
	FConsoleCommandDelegate::CreateStatic(UCoreAssetManager::DumpLoadedAssets)
);

////////////////////////////////////////////////////////////////////

#define STARTUP_JOB_WEIGHTED(JobFunc, JobWeight) StartupJobs.Add(FCoreAssetManagerStartupJob(#JobFunc, [this](const FCoreAssetManagerStartupJob& StartupJob, TSharedPtr<FStreamableHandle>& LoadHandle){JobFunc;},JobWeight))
#define STARTUP_JOB(JobFunc) STARTUP_JOB_WEIGHTED(JobFunc, 1.f)

////////////////////////////////////////////////////////////////////

UCoreAssetManager::UCoreAssetManager()
{
	DefaultPawnData = nullptr;
}

UCoreAssetManager& UCoreAssetManager::Get()
{
	check(GEngine);
	
	if (UCoreAssetManager* Singleton = Cast<UCoreAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}
	
	UE_LOG(Log_AssetManager,Fatal,TEXT("Invalid AssetManagerClassName in DefaultEngine.ini. It must be set to CoreAssetManager!"))
	
	//Fatal error above prevents this from being called.
	return *NewObject<UCoreAssetManager>();
}

void UCoreAssetManager::DumpLoadedAssets()
{
	UE_LOG(Log_AssetManager,Log,TEXT("================ Start Dumping Loaded Assets ==================="));
	
	for (const UObject* LoadedAsset : Get().LoadedAssets)
	{
		UE_LOG(Log_AssetManager,Log,TEXT(" %s"),*GetNameSafe(LoadedAsset));
	}
	
	UE_LOG(Log_AssetManager,Log,TEXT("... %d assets in loaded pool"),Get().LoadedAssets.Num());
	UE_LOG(Log_AssetManager,Log,TEXT("=============== Finish Dumping Loaded Assets ==============="));
}

const UCoreGameData& UCoreAssetManager::GetGameData()
{
	return GetOrLoadTypedGameData<UCoreGameData>(CoreGameDataPath);
}

const UProjectPawnData* UCoreAssetManager::GetDefaultPawnData()
{
	return GetAsset(DefaultPawnData);
}

UObject* UCoreAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;

		if (ShouldLogAssetLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Syncronously loaded asset [%s]"),*AssetPath.ToString()),nullptr);
		}

		if (UAssetManager::IsInitialized())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath,false);
		}
		
		//Use LoadObject if asset manager isn't ready yet.
		return AssetPath.TryLoad();
	}
	
	return nullptr;
}

bool UCoreAssetManager::ShouldLogAssetLoads()
{
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

void UCoreAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}

void UCoreAssetManager::StartInitialLoading()
{
	SCOPED_BOOT_TIMING("UCoreAssetManager::StartInitialLoading");
	
	//This does all of the scanning, need to do this now even if loads are deferred
	Super::StartInitialLoading();
	
	STARTUP_JOB(UAbilitySystemGlobals::Get().InitGlobalData());
	
	{
		//Load base game data asset
		STARTUP_JOB_WEIGHTED(GetGameData(),25.f);
	}
	
	STARTUP_JOB(InitializeGameplayCueManager());
	
	//Run all the queued up startup jobs
	DoAllStartupJobs();
}

#if WITH_EDITOR
void UCoreAssetManager::PreBeginPIE(bool bStartSimulate)
{
	Super::PreBeginPIE(bStartSimulate);
	
	{
		FScopedSlowTask SlowTask(0,NSLOCTEXT("ModularProjectEditor","BeginLoadingPIEData","Loading PIE Data"));
		const bool bShowCancelButton = false;
		const bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);
		
		const UCoreGameData& LocalGameDataCommon = GetGameData();
		
		//Intentionally after GetGameData to avoid counting GameData time in this timer
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("PreBeginPIE asset preloading complete"),nullptr);
		
		//You could add preloading of anything else needed for the experience we'll be using here
		//(e.g., by grabbing the default experience from the world settings + the experience override in developer settings)
	}
}
#endif

UPrimaryDataAsset* UCoreAssetManager::LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass,
	const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType)
{
	if (DataClassPath.IsNull())
	{
		UE_LOG(Log_AssetManager,Fatal,TEXT("DataClassPath is NULL for type %s. Check your DefaultEngine.ini configuration!"),*PrimaryAssetType.ToString());
		return nullptr;
	}
	
	UPrimaryDataAsset* Asset = nullptr;
	
	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Loading GameData Object"), STAT_GameData, STATGROUP_LoadTime);
	if (!DataClassPath.IsNull())
	{
#if WITH_EDITOR
		FScopedSlowTask SlowTask(0,FText::Format(NSLOCTEXT("ModularProjectEditor","BeginLoadingGameTask","Loading GameData {0}"),FText::FromName(DataClass->GetFName())));
		const bool bShowCancelButton = false;
		const bool bAllowInPIE = true;
		SlowTask.MakeDialog(bShowCancelButton, bAllowInPIE);
#endif
		UE_LOG(Log_AssetManager,Log,TEXT("Loading GameData: %s ..."),*DataClassPath.ToString());
		SCOPE_LOG_TIME_IN_SECONDS(TEXT("    ... GameData loaded!"),nullptr);
		
		//This can be called recursively in editor because it is called on demand from PostLoad so force a sync load for primary asset and async load the rest in that case
		if (GIsEditor)
		{
			Asset = DataClassPath.LoadSynchronous();
			LoadPrimaryAssetsWithType(PrimaryAssetType);
		}
		else
		{
			TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
			if (Handle.IsValid())
			{
				Handle->WaitUntilComplete(0.0f,false);
				
				//This should always work
				Asset = Cast<UPrimaryDataAsset>(Handle->GetLoadedAsset());
			}
		}
	}

	if (Asset)
	{
		GameDataMap.Add(DataClass,Asset);
	}
	else
	{
		// It is not acceptable to fall to load any GaneData asset. It will result in soft failures that are hard to diagnose.
		UE_LOG(Log_AssetManager,Fatal,TEXT("Failed to load GameData asset at %s. Type %s. This is not recoverable and likely means you do not have the correct data to run %s"),*DataClassPath.ToString(),*PrimaryAssetType.ToString(),FApp::GetProjectName());
	}
	
	return Asset;
}

void UCoreAssetManager::DoAllStartupJobs()
{
	SCOPED_BOOT_TIMING("UCoreAssetManager::DoAllStartupJobs");
	const double AllStartupJobsTime = FPlatformTime::Seconds();

	if (IsRunningDedicatedServer())
	{
		//No need for periodic progress updates, just run the jobs
		for (const FCoreAssetManagerStartupJob& StartupJob : StartupJobs)
		{
			StartupJob.DoJob();
		}
	}
	else
	{
		if (StartupJobs.Num() >0)
		{
			float TotalJobValue = 0.0f;
			for (const FCoreAssetManagerStartupJob& StartupJob : StartupJobs)
			{
				TotalJobValue += StartupJob.JobWeight;
			}
			
			float AccumulatedJobValue = 0.0f;
			for (FCoreAssetManagerStartupJob& StartupJob : StartupJobs)
			{
				const float JobValue = StartupJob.JobWeight;
				StartupJob.SubstepProgressDelegate.BindLambda([This = this, AccumulatedJobValue,JobValue,TotalJobValue](float NewProgress)
				{
					const float SubstepAdjustment = FMath::Clamp(NewProgress, 0.f, 1.f) * JobValue;
					const float OverallPercentWithSubstep = (AccumulatedJobValue + SubstepAdjustment) / TotalJobValue;
					
					This->UpdateInitialGameContentLoadPercent(OverallPercentWithSubstep);
				});
				StartupJob.DoJob();
				
				StartupJob.SubstepProgressDelegate.Unbind();
				
				AccumulatedJobValue += JobValue;
				
				UpdateInitialGameContentLoadPercent(AccumulatedJobValue/TotalJobValue);
			}
		}
		else
		{
			UpdateInitialGameContentLoadPercent(1.f);
		}
	}
	StartupJobs.Empty();
	
	UE_LOG(Log_AssetManager,Display,TEXT("All startup jobs took %.2f seconds to complete"),FPlatformTime::Seconds() - AllStartupJobsTime);
}

void UCoreAssetManager::InitializeGameplayCueManager()
{
	SCOPED_BOOT_TIMING("UCoreAssetManager::InitializeGameplayCueManager");
	
	UCoreGameplayCueManager* GCM = UCoreGameplayCueManager::Get();
	check(GCM);
	GCM->LoadAlwaysLoadedCues();
}

void UCoreAssetManager::UpdateInitialGameContentLoadPercent(float GameContentPercent)
{
	//Could route this to the early startup loading screen
}
