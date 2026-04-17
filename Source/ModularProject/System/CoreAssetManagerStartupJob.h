#pragma once
#include "Engine/StreamableManager.h"

DECLARE_DELEGATE_OneParam(FCoreAssetManagerStartupJobSubstemProgress, float /*NewProgress*/);

/** Handles reporting from streamable handles */
struct FCoreAssetManagerStartupJob
{
	FCoreAssetManagerStartupJobSubstemProgress SubstepProgressDelegate;
	TFunction<void(const FCoreAssetManagerStartupJob&, TSharedPtr<FStreamableHandle>&)> JobFunc;
	FString JobName;
	float JobWeight;
	mutable  double LastUpdate = 0;
	
	/** Simple job that all synchronous */
	FCoreAssetManagerStartupJob(const FString& InJobName, const TFunction<void(const FCoreAssetManagerStartupJob&, TSharedPtr<FStreamableHandle>&)>& InJobFunc, float InJobWeight)
		: JobFunc(InJobFunc)
	, JobName(InJobName)
	, JobWeight(InJobWeight)
	{}
	
	/** Perform actual loading, will return a handle if it created one */
	TSharedPtr<FStreamableHandle> DoJob() const;
	
	void UpdateSubstepProgress(float NewProgress) const
	{
		SubstepProgressDelegate.ExecuteIfBound(NewProgress);
	}
	
	void UpdateSubstepProgressFromStreamable(TSharedRef<FStreamableHandle> StreamableHandle) const
	{
		if (SubstepProgressDelegate.IsBound())
		{
			double Now = FPlatformTime::Seconds();
			if (Now - LastUpdate > 1.0 / 60)
			{
				SubstepProgressDelegate.Execute(StreamableHandle->GetProgress());
				LastUpdate = Now;
			}
		}
	}
};

