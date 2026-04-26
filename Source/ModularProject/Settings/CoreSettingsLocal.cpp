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


UCoreSettingsLocal::UCoreSettingsLocal()
{
}

UCoreSettingsLocal* UCoreSettingsLocal::Get()
{
}

void UCoreSettingsLocal::BeginDestroy()
{
	Super::BeginDestroy();
}

void UCoreSettingsLocal::SetToDefaults()
{
	Super::SetToDefaults();
}

void UCoreSettingsLocal::LoadSettings(bool bForceReload)
{
	Super::LoadSettings(bForceReload);
}

void UCoreSettingsLocal::ConfirmVideoMode()
{
	Super::ConfirmVideoMode();
}

float UCoreSettingsLocal::GetEffectiveFrameRateLimit()
{
	return Super::GetEffectiveFrameRateLimit();
}

void UCoreSettingsLocal::ResetToCurrentSettings()
{
	Super::ResetToCurrentSettings();
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
