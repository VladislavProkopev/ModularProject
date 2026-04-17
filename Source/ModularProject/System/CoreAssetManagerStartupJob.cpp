#include "CoreAssetManagerStartupJob.h"
//TODO подумать нужны ли LogChannels

DEFINE_LOG_CATEGORY_STATIC(Log_CoreAssetManager,All,All);

TSharedPtr<FStreamableHandle> FCoreAssetManagerStartupJob::DoJob() const
{
	const double JobStartTime = FPlatformTime::Seconds();
	
	TSharedPtr<FStreamableHandle> Handle;
	UE_LOG(Log_CoreAssetManager,Display,TEXT("Startup job \"%s\" starting"),*JobName);
	JobFunc(*this,Handle);

	if (Handle.IsValid())
	{
		Handle->BindUpdateDelegate(FStreamableUpdateDelegate::CreateRaw(this,&FCoreAssetManagerStartupJob::UpdateSubstepProgressFromStreamable));
		Handle->WaitUntilComplete(0.0f,false);
		Handle->BindUpdateDelegate(FStreamableUpdateDelegate());
	}
	
	UE_LOG(Log_CoreAssetManager,Display,TEXT("Startup job \"%s\" took %.2f seconds to complete"),*JobName,FPlatformTime::Seconds() - JobStartTime);
	
	return Handle;
}
