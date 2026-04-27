// Fill out your copyright notice in the Description page of Project Settings.

#include "CoreSettingsLocal.h"
#include "Engine/Engine.h"
#include "Framework/Application/SlateApplication.h"
#include "Engine/World.h"
#include "Misc/App.h"
#include "CommonInputSubsystem.h"
#include "GenericPlatform/GenericPlatformFramePacer.h"
//TODO ModularProject/Player/CoreLocalPlayer.h
#include "ModularProject/Player/CoreLocalPlayer.h"
#include "Performance/LatencyMarkerModule.h"
//TODO ModularProject/Performance/CorePrfomanceStateTypes.h
#include "ModularProject/Performance/CorePrfomanceStateTypes.h"
#include "ICommonUIModule.h"
#include "CommonUISettings.h"
#include "SoundControlBusMix.h"
#include "Widgets/Layout/SSafeZone.h"
//TODO ModularProject/Performance/CorePerformanceSettings.h
#include "ModularProject/Performance/CorePerformanceSettings.h"
#include "DeviceProfiles/DeviceProfileManager.h"
#include "DeviceProfiles/DeviceProfile.h"
#include "HAL/PlatformFramePacer.h"
//TODO ModularProject/Development/CorePlatformEmulationSettings.h
#include "ModularProject/Development/CorePlatformEmulationSettings.h"
#include "SoundControlBus.h"
#include "AudioModulationStatics.h"
//TODO ModularProject/Audio/CoreAudioSettings.h
#include "ModularProject/Audio/CoreAudioSettings.h"
//TODO ModularProject//Audio/CoreAudioMixEffectsSubsystem.h
#include "ModularProject/Audio/CoreAudioMixEffectsSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CoreSettingsLocal)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_BinauralSettingControlledByOS,"Platform.Trait.BinauralSettingControlledByOS")

namespace PerfStatTags
{
	UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_SupportsLatencyStats,"Platform.Trait.SupportsLatencyStats");
	UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_SupportsLatencyMarkers,"Platform.Trait.SupportsLatencyMarkers");
}

//////////////////////////////////////////////////////////////////////////////
#if WITH_EDITOR
static TAutoConsoleVariable<bool> CVarApplyFramerateSettingsInPIE(TEXT("Core.Settings.ApplyFramerateSettingsInPIE"),
	false,
	TEXT("Should we apply frame rate settings in PIE?"),
	ECVF_Default);

static TAutoConsoleVariable<bool> CVarApplyFrontEndPerfomanceOptionsInPIE(TEXT("Core.Settings.ApplyFrontEndPerfomanceOptions"),
	false,
	TEXT("Do we apply front-end specific performance options in PIE?"),
	ECVF_Default);

static TAutoConsoleVariable<bool> CVarApplyDeviceProfilesInPIE(TEXT("Core.Settings.ApplyDeviceProfilesInPIE"),
	false,
	TEXT("Should we apply experience/platform emulated device profiles in PIE?"),
	ECVF_Default);
#endif
//////////////////////////////////////////////////////////////////////////////
//Console frame packing

static TAutoConsoleVariable<int32> CVarDevineProfileDrivenTargetFps(
	TEXT("Core.DeviceProfile.Console.TargerFPS"),
	-1,
	TEXT("Target FPS when being driven by device profile"),
	ECVF_Default|ECVF_Preview);

static TAutoConsoleVariable<int32> CVarDeviceProfileDrivenFrameSyncType(
	TEXT("Core.DeviceProfile.Mobile.DefaultFrameRate"),
	30,
	TEXT("Max FPS when being driven by device profile"),
	ECVF_Default|ECVF_Preview);
//////////////////////////////////////////////////////////////////////////////

static TAutoConsoleVariable<FString> CVarMobileQualityLimits(
	TEXT("Core.DeviceProfile.Mobile.OveralQualityLimits"),
	TEXT(""),
	TEXT("List of limits on resolution quality of the from \"FPS:MaxQuality,FPS2:MaxQuality2,...\", kicking in when FPS is at or above the threshold"),
	ECVF_Default|ECVF_Preview);

static TAutoConsoleVariable<FString> CVarMobileResolutionQualityLimits(
	TEXT("Core.DeviceProfile.Mobile.OveralResolutionQualityLimits"),
	TEXT(""),
	TEXT("List of limits on resolution quality of the from \"FPS:MaxResQuality,FPS2:MaxResQuality2,...\", kicking in when FPS is at or above the threshold"),
	ECVF_Default|ECVF_Preview);

static TAutoConsoleVariable<FString> CVarMobileResolutionQualityRecommendation(
	TEXT("Core.DeviceProfile.Mobile.ResolutionQualityRecommendation"),
	TEXT(""),
	TEXT("List of limits on resolution quality of thw from \"FPS:Recommendation,FPS2:Recommendation,...\", kicking in when FPS is at or above the threshold"),
	ECVF_Default|ECVF_Preview);
//////////////////////////////////////////////////////////////////////////////

FCoreScalabilitySnapshot::FCoreScalabilitySnapshot()
{
	static_assert(sizeof(Scalability::FQualityLevels) == 88, "This function may need to be updated to account for new members");
	
	Qualities.ResolutionQuality = -1.f;
	Qualities.ViewDistanceQuality = -1;
	Qualities.AntiAliasingQuality = -1;
	Qualities.ShadowQuality = -1;
	Qualities.GlobalIlluminationQuality = -1;
	Qualities.ReflectionQuality = -1;
	Qualities.PostProcessQuality = -1;
	Qualities.TextureQuality = -1;
	Qualities.EffectsQuality = -1;
	Qualities.FoliageQuality = -1;
	Qualities.ShadingQuality = -1;
}
//////////////////////////////////////////////////////////////////////////////

template<typename T>
struct TMobileQualityWrapper
{
private:
	T DefaultValue;
	TAutoConsoleVariable<FString>& WathedVar;
	FString LastSeenCVarString;

	struct FLimitPair
	{
		int32 Limit = 0;
		T Value = T(0);
	};
	
	TArray<FLimitPair> Thresholds;
public:
	TMobileQualityWrapper(T InDefaultValue,TAutoConsoleVariable<FString>& InWatchedVar): DefaultValue(InDefaultValue),WathedVar(InWatchedVar){}
	
	T Querry(int32 TestValue)
	{
		UpdateCache();
		for (const FLimitPair& Pair : Thresholds)
		{
			if (TestValue >= Pair.Limit)
			{
				return Pair.Value;
			}
		}
		return DefaultValue;
	}
	
	//Returns the first threshold value or INDEX_NONE if there aren't any
	int32 GetFirstTHreshold()
	{
		UpdateCache();
		return (Thresholds.Num() > 0) ? Thresholds[0].Limit : INDEX_NONE;
	}
	
	//Returns the lowest value of all the pairs or DefaultIfNoPairs if there are no pairs
	T GetLowestValue(T DefaultIfNoPairs)
	{
		UpdateCache();
		
		T Result = DefaultIfNoPairs;
		bool bFirstValue = true;
		for (const FLimitPair& Pair : Thresholds)
		{
			if (bFirstValue)
			{
				Result = Pair.Value;
				bFirstValue = false;
			}
			else
			{
				Result = FMath::Min(Result,Pair.Value);
			}
		}
		return Result;
	}
	
private:
	void UpdateCache()
	{
		const FString CurrentValue = WathedVar.GetValueOnGameThread();
		if (!CurrentValue.Equals(LastSeenCVarString,ESearchCase::CaseSensitive))
		{
			LastSeenCVarString = CurrentValue;
			
			Thresholds.Reset();
			
			// Parse the thresholds
			int32 ScanIndex = 0;
			while (ScanIndex < LastSeenCVarString.Len())
			{
				const int32 ColumnIndex = LastSeenCVarString.Find(TEXT(":"),ESearchCase::CaseSensitive,ESearchDir::FromStart,ScanIndex);
				if (ColumnIndex > 0)
				{
					const int32 CommaIndex = LastSeenCVarString.Find(TEXT(","),ESearchCase::CaseSensitive,ESearchDir::FromStart,ColumnIndex);
					const int32 EndOfPairIndex = (CommaIndex != INDEX_NONE) ? CommaIndex : LastSeenCVarString.Len();
					
					FLimitPair Pair;
					LexFromString(Pair.Limit,*LastSeenCVarString.Mid(ScanIndex,ColumnIndex - ScanIndex));
					LexFromString(Pair.Value,*LastSeenCVarString.Mid(CommaIndex + 1,EndOfPairIndex - ColumnIndex - 1));
					Thresholds.Add(Pair);
					
					ScanIndex = EndOfPairIndex + 1;
				}
				else
				{
					UE_LOG(LogConsoleResponse,Error,TEXT("Malformed value for '%s'='%s', expecting a ':'"),
						*IConsoleManager::Get().FindConsoleObjectName(WathedVar.AsVariable()),
						*LastSeenCVarString);
					Thresholds.Reset();
					break;
				}
			}
			
			// Sort the pairs
			Thresholds.Sort([](const FLimitPair& A, const FLimitPair& B){return A.Limit < B.Limit;});
		}
	}
};

namespace CoreSettingsHelpers
{
	bool HasPlatformTrait(FGameplayTag Tag)
	{
		return ICommonUIModule::GetSettings().GetPlatformTraits().HasTag(Tag);
	}
	
	//Returns the max level from integer scalability settings(ignores ResolutionQuality)
	int32 GetHiestLevelOfAnyScalabilityChannel(const Scalability::FQualityLevels& ScalabilityQuality)
	{
		static_assert(sizeof(Scalability::FQualityLevels) == 88, "This function may need to be updated to account for new members");
		
		int32 MaxScalability = ScalabilityQuality.ViewDistanceQuality;
		MaxScalability = FMath::Max(MaxScalability,ScalabilityQuality.AntiAliasingQuality);
		MaxScalability = FMath::Max(MaxScalability,ScalabilityQuality.ShadowQuality);
		MaxScalability = FMath::Max(MaxScalability,ScalabilityQuality.GlobalIlluminationQuality);
		MaxScalability = FMath::Max(MaxScalability,ScalabilityQuality.ReflectionQuality);
		MaxScalability = FMath::Max(MaxScalability,ScalabilityQuality.PostProcessQuality);
		MaxScalability = FMath::Max(MaxScalability,ScalabilityQuality.TextureQuality);
		MaxScalability = FMath::Max(MaxScalability,ScalabilityQuality.EffectsQuality);
		MaxScalability = FMath::Max(MaxScalability,ScalabilityQuality.FoliageQuality);
		MaxScalability = FMath::Max(MaxScalability,ScalabilityQuality.ShadingQuality);
		
		return (MaxScalability >=0)?MaxScalability : -1;
	}
	
	void FillScalabilitySettingsFromDeviceProfile(FCoreScalabilitySnapshot& Mode, const FString& Suffix = FString())
	{
		static_assert(sizeof(Scalability::FQualityLevels) == 88, "This function may need updated to account for new members");
		
		//Default out before filling so we can correctly mark non-overriden scalability values.
		//It's technically possible to swap device profile when testing so safest to clear and refill
		Mode = FCoreScalabilitySnapshot();
		
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.ResolutionQuality%s"),*Suffix),Mode.Qualities.ResolutionQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.ViewDistanceQuality%s"),*Suffix),Mode.Qualities.ViewDistanceQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.AntiAliasingQuality%s"),*Suffix),Mode.Qualities.AntiAliasingQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.ShadowQuality%s"),*Suffix),Mode.Qualities.ShadowQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.GlobalIlluminationQuality%s"),*Suffix),Mode.Qualities.GlobalIlluminationQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.ReflectionQuality%s"),*Suffix),Mode.Qualities.ReflectionQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.PostProcessQuality%s"),*Suffix),Mode.Qualities.PostProcessQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.TextureQuality%s"),*Suffix),Mode.Qualities.TextureQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.EffectsQuality%s"),*Suffix),Mode.Qualities.EffectsQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.FoliageQuality%s"),*Suffix),Mode.Qualities.FoliageQuality);
		Mode.bHasOverrides |= UDeviceProfileManager::GetScalabilityCVar(FString::Printf(TEXT("sg.ShadingQuality%s"),*Suffix),Mode.Qualities.ShadingQuality);
	}
	
	TMobileQualityWrapper<int32> OverallQualityLimits(-1,CVarMobileQualityLimits);
	TMobileQualityWrapper<float> ResolutionQualityLimits(100.f,CVarMobileResolutionQualityLimits);
	TMobileQualityWrapper<float> ResolutionQualityRecommendation(75.f,CVarMobileResolutionQualityRecommendation);
	
	int32 GetApplicableOverallQualityLimit(int32 FrameRate)
	{
		return OverallQualityLimits.Querry(FrameRate);
	}
	
	float GetApplicableResolutionQualityLimit(int32 FrameRate)
	{
		return ResolutionQualityLimits.Querry(FrameRate);
	}
	
	float GetApplicableResolutionQualityRecommendation(int32 FrameRate)
	{
		return ResolutionQualityRecommendation.Querry(FrameRate);
	}
	
	int32 ConstrainFrameRateToBeCompatibleWithOverallQuality(int32 FrameRate, int32 OverallQuality)
	{
		//TODO UCorePlatformSpecificRenderSettings
		const UCorePlatformSpecificRenderSettings* PlatformSettings = UCorePlatformSpecificRenderSettings::Get();
		const TArray<int32>& PossibleRates = PlatformSettings->MobileFrameRateLimits;
		
		//Choose the closest frame rate (without going over) to the user preferred one that is supported and compatible with the desired overall quality
		int32 LimitIndex = PossibleRates.FindLastByPredicate([=](const int32& TestRate)
		{
			const bool bAtOrBelowDesiredRate = (TestRate <= FrameRate);
			
			const int32 LimitQuality = GetApplicableResolutionQualityLimit(TestRate);
			const bool bQualityDosentExceedLimit = (LimitQuality < 0) || (OverallQuality <= LimitQuality);
			
			const bool bIsSupported = UCoreSettingsLocal::IsSupportedMobileFramePace(TestRate);
			
			return bAtOrBelowDesiredRate && bQualityDosentExceedLimit && bIsSupported;
		});
		
		return PossibleRates.IsValidIndex(LimitIndex) ? PossibleRates[LimitIndex] : UCoreSettingsLocal::GetDefaultMobileFrameRate();
	}
	
	//Returns the first rfame rate at which overall quality is restricted/limited by the current device profile
	int32 GetFirstFrameRateWithQualityLimit()
	{
		return OverallQualityLimits.GetFirstTHreshold();
	}
	
	//Returns the lowest quality at which there's a limit on the overall frame rate (or -1 if there is no limit)
	int32 GetLowestQualityFrameRateLimit()
	{
		return OverallQualityLimits.GetLowestValue(-1);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///

UCoreSettingsLocal::UCoreSettingsLocal()
{
	if (!HasAnyFlags(RF_ClassDefaultObject) && FSlateApplication::IsInitialized())
	{
		OnApplicationActivationStateChangedHandle = FSlateApplication::Get().OnApplicationActivationStateChanged().AddUObject(this, &ThisClass::OnAppActivationStateChanged);
	}
	
	bEnableScalabilitySettings = UCorePlatformSpecificSettings::Get()->bSupportsGranularVideoQualitySettings;
	
	SetToDefaults();
}

UCoreSettingsLocal* UCoreSettingsLocal::Get()
{
	return GEngine ? CastChecked<UCoreSettingsLocal>(GEngine->GetGameUserSettings()) : nullptr;
}

void UCoreSettingsLocal::BeginDestroy()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().OnApplicationActivationStateChanged().Remove(OnApplicationActivationStateChangedHandle);
	}
	Super::BeginDestroy();
}

void UCoreSettingsLocal::SetToDefaults()
{
	Super::SetToDefaults();
	
	bUseHeadphoneMode = false;
	bUseHDRAudioMode = false;
	bSoundControllBusMixLoaded = false;
	bEnableLatencyTrackingStats = UCoreSettingsLocal::DoesPlatformSupportLatencyTrackingStats();
	
	const UCorePlatformSpecificRenderSettings* PlatformSettings = UCorePlatformSpecificRenderSettings::Get();
	UserChosenDeviceProfileSuffix = PlatformSettings->DefaultDeviceProfileSuffix;
	DesiredUserChosenDeviceProfileSuffix = UserChosenDeviceProfileSuffix;
	
	FrameRateLimit_InMenu = 144.f;
	FrameRateLimit_WhenBackgrounded = 30.f;
	FrameRateLimit_OnBattery = 60.f;
	
	MobileFrameRateLimit = GetDefaultMobileFrameRate();
	DesiredMobileFrameRateLimit = MobileFrameRateLimit;
}

void UCoreSettingsLocal::LoadSettings(bool bForceReload)
{
	Super::LoadSettings(bForceReload);
	
	//Console platforms use rhi.SyncInterval to lomit framerate
	const UCorePlatformSpecificRenderSettings* PlatformSettings = UCorePlatformSpecificRenderSettings::Get();
	//TODO ECoreFramePacingMode
	if (PlatformSettings->FramePacingMode == ECoreFramePacingMode::ConsoleStyle)
	{
		FrameRateLimit = 0.f;
	}
	
	//Enable HRTF if needed
	bDesiredHeadphoneMode = bUseHeadphoneMode;
	SetHeadphoneModeEnabled(bUseHeadphoneMode);
	
	ApplyLatencyTrackingStatsSettings();
	
	DesiredUserChosenDeviceProfileSuffix = UserChosenDeviceProfileSuffix;
	
	CoreSettingsHelpers::FillScalabilitySettingsFromDeviceProfile(DeviceDefaultScalabilitySettings);
	
	DesiredMobileFrameRateLimit = MobileFrameRateLimit;
	ClampMobileQuality();
	
	PerfStatSettingsChangedEvent.Broadcast();
}

void UCoreSettingsLocal::ConfirmVideoMode()
{
	Super::ConfirmVideoMode();
	
	SetMobileFPSMode(DesiredMobileFrameRateLimit);
}

//Combine two limits, always taking the minimum of the two (with special handling for values of <=0 meaning unlimited)
float CombineFrameRateLimits(float Limit1, float Limit2)
{
	if (Limit1 <= 0.f)
	{
		return Limit2;
	}
	else if (Limit2 <= 0.f)
	{
		return Limit1;
	}
	else
	{
		return FMath::Min(Limit1, Limit2);
	}
}

float UCoreSettingsLocal::GetEffectiveFrameRateLimit()
{
	const UCorePlatformSpecificRenderSettings* PlatformSettings = UCorePlatformSpecificRenderSettings::Get();
	
#if WITH_EDITOR
	if (GIsEditor && !CVarApplyFramerateSettingsInPIE.GetValueOnGameThread())
	{
		return Super::GetEffectiveFrameRateLimit();
	}
#endif

	if (PlatformSettings->FramePacingMode == ECoreFramePacingMode::ConsoleStyle)
	{
		return 0.f;
	}
	
	float EffectiveFrameRateLimit = Super::GetEffectiveFrameRateLimit();

	if (ShouldUseFrontendPerformanceSettings())
	{
		EffectiveFrameRateLimit = CombineFrameRateLimits(EffectiveFrameRateLimit, FrameRateLimit_InMenu);
	}
	
	//TODO Продолжить отсюда
	
}

void UCoreSettingsLocal::ResetToCurrentSettings()
{
	Super::ResetToCurrentSettings();
	
	bDesiredHeadphoneMode = bUseHeadphoneMode;
	UserChosenDeviceProfileSuffix = DesiredUserChosenDeviceProfileSuffix;
	MobileFrameRateLimit = DesiredMobileFrameRateLimit;
}

void UCoreSettingsLocal::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();
}

int32 UCoreSettingsLocal::GetOverallScalabilityLevel() const
{
	return Super::GetOverallScalabilityLevel();
}

void UCoreSettingsLocal::SetOverallScalabilityLevel(int32 Value)
{
	Super::SetOverallScalabilityLevel(Value);
}

void UCoreSettingsLocal::OnExperienceLoaded()
{
}

void UCoreSettingsLocal::OnHotfixDeviceProfileApplied()
{
}

void UCoreSettingsLocal::SetShouldUseFrontendPerformanceSettings(bool bIsFrontEnd)
{
}

bool UCoreSettingsLocal::ShouldUseFrontendPerformanceSettings()
{
}

ECoreStatDisplayMode UCoreSettingsLocal::GetPerfStatDisplayState(ECoreDisplayablePerfomanceStat Stat) const
{
}

void UCoreSettingsLocal::SetPerfStatDisplayState(ECoreDisplayablePerfomanceStat Stat, ECoreStatDisplayMode DisplayMode)
{
}

bool UCoreSettingsLocal::DoesPlatformSupportLatencyMarkers()
{
}

void UCoreSettingsLocal::SetEnableLatencyFlashIndicators(const bool NewVal)
{
}

bool UCoreSettingsLocal::DoesPlatformSupportLatencyTrackingStats()
{
}

void UCoreSettingsLocal::SetEnableLatencyTrackingStats(const bool NewVal)
{
}

void UCoreSettingsLocal::ApplyLatencyTrackingStatsSettings()
{
}

float UCoreSettingsLocal::GetDisplayGamma() const
{
}

void UCoreSettingsLocal::SetDisplayGamma(float InGamma)
{
}

void UCoreSettingsLocal::ApplyDisplayGamma()
{
}

float UCoreSettingsLocal::GetFrameRateLimit_OnBattery() const
{
}

void UCoreSettingsLocal::SetFrameRateLimit_OnBattery(float NewLimitFPS)
{
}

float UCoreSettingsLocal::GetFrameRateLimit_InMenu() const
{
}

void UCoreSettingsLocal::SetFrameRateLimit_InMenu(float NewLimitFPS)
{
}

float UCoreSettingsLocal::GetFrameRateLimit_WhenBackgrounded() const
{
}

void UCoreSettingsLocal::SetFrameRateLimit_WhenBackgrounded(float NewLimitFPS)
{
}

float UCoreSettingsLocal::GetFrameRateLimit_Always() const
{
}

void UCoreSettingsLocal::SetFrameRateLimit_Always(float NewLimitFPS)
{
}

void UCoreSettingsLocal::UpdateEffectiveFrameRateLimit()
{
}

int32 UCoreSettingsLocal::GetDefaultMobileFrameRate()
{
}

int32 UCoreSettingsLocal::GetMaxMobileFrameRate()
{
}

bool UCoreSettingsLocal::IsSupportedMobileFramePace(int32 TestFPS)
{
}

int32 UCoreSettingsLocal::GetFirstFrameRateWithQualityLimit() const
{
}

int32 UCoreSettingsLocal::GetLowestQualityWithFrameRateLimit() const
{
}

void UCoreSettingsLocal::ResetToMobileDeviceDefaults()
{
}

int32 UCoreSettingsLocal::GetMaxSupportedOverallQualityLevel() const
{
}

void UCoreSettingsLocal::SetMobileFPSMode(int32 NewLimitFPS)
{
}

void UCoreSettingsLocal::ClampMobileResolutionQuality(int32 TargetFPS)
{
}

void UCoreSettingsLocal::RemapMobileResolutionQuality(int32 FromFPS, int32 ToFPS)
{
}

void UCoreSettingsLocal::ClampMobileFPSQualityLevels(bool bWriteBack)
{
}

void UCoreSettingsLocal::ClampMobileQuality()
{
}

int32 UCoreSettingsLocal::GetHighestLevelOfAntScalabilityChannel() const
{
}

void UCoreSettingsLocal::OverrideQualityLevelsToScalabilityMode(const FCoreScalabilitySnapshot& InMode,
	Scalability::FQualityLevels& InOutLevels)
{
}

void UCoreSettingsLocal::ClampQualityLevelsToDeviceProfile(const Scalability::FQualityLevels& ClampLevels,
	Scalability::FQualityLevels& InOutLevels)
{
}

void UCoreSettingsLocal::SetDesiredMobileFrameRateLimit(int32 NewLimitFPS)
{
}

FString UCoreSettingsLocal::GetDesiredDeviveProfileQualitySuffix() const
{
}

void UCoreSettingsLocal::SetDesiredDeviveProfileQualitySuffix(const FString& InDesiredSuffix)
{
}

void UCoreSettingsLocal::UpdateGameModeDeviceProfileAndFps()
{
}

void UCoreSettingsLocal::UpdateConsoleFramePacing()
{
}

void UCoreSettingsLocal::UpdateDesktopFramePacing()
{
}

void UCoreSettingsLocal::UpdateMobileFramePacing()
{
}

void UCoreSettingsLocal::UpdateDynamicResFrameTime(float TargetFPS)
{
}

bool UCoreSettingsLocal::IsHeadphoneModeEnabled() const
{
}

void UCoreSettingsLocal::SetHeadphoneModeEnabled(bool bEnabled)
{
}

bool UCoreSettingsLocal::CanModifyHeadphoneModeEnabled() const
{
}

bool UCoreSettingsLocal::IsHDRAudioModeEnabled() const
{
}

void UCoreSettingsLocal::SetHDRAudioModeEnabled(bool bEnabled)
{
}

bool UCoreSettingsLocal::CanRunAutoBenchmark() const
{
}

bool UCoreSettingsLocal::ShouldRunAutoBenchmarkAtStartup() const
{
}

void UCoreSettingsLocal::RunAutoBenchmark(bool bSaveImmediately)
{
}

void UCoreSettingsLocal::ApplyScalabilitySettings()
{
}

float UCoreSettingsLocal::GetOverallVolume() const
{
}

void UCoreSettingsLocal::SetOverallVolume(float InVolume)
{
}

float UCoreSettingsLocal::GetMusicVolume() const
{
}

void UCoreSettingsLocal::SetMusicVolume(float InVolume)
{
}

float UCoreSettingsLocal::GetSoundFXVolume() const
{
}

void UCoreSettingsLocal::SetSoundFXVolume(float InVolume)
{
}

float UCoreSettingsLocal::GetDialogueVolume() const
{
}

void UCoreSettingsLocal::SetDialogueVolume(float InVolume)
{
}

float UCoreSettingsLocal::GetVoiceChatVolume() const
{
}

void UCoreSettingsLocal::SetVoiceChatVolume(float InVolume)
{
}

FString UCoreSettingsLocal::GetAudioOutputDeviceId() const
{
}

void UCoreSettingsLocal::SetAudioOutputDeviceId(const FString& InAudioOutputDeviceId)
{
}

void UCoreSettingsLocal::SetVolumeForSoundClass(FName ChannelName, float InVolume)
{
}

void UCoreSettingsLocal::ApplySafeZoneScale()
{
}

void UCoreSettingsLocal::SetVolumeForControlBus(USoundControlBus* InControlBus, float InVolume)
{
}

void UCoreSettingsLocal::SetControllerPlatform(const FName InControllerPlatform)
{
}

FName UCoreSettingsLocal::GetControllerPlatform() const
{
}

void UCoreSettingsLocal::LoadUserControllBusMix()
{
}

void UCoreSettingsLocal::OnAppActivationStateChanged(bool bIsActive)
{
}

void UCoreSettingsLocal::ReapplyThingDueToPossibleDeviceProfileChange()
{
}
