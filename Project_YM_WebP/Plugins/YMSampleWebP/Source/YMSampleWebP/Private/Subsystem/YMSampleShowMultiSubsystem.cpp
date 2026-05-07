#include "YMSampleShowMultiSubsystem.h"
#include "YMSampleWebPCore.h"
#include "YMSampleWebPLog.h"
#include "Misc/Paths.h"
#include "Async/Async.h"

bool UYMSampleShowMultiSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UYMSampleShowMultiSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UYMSampleShowMultiSubsystem::Deinitialize()
{
	FreeWebp();
	Super::Deinitialize();
}

void UYMSampleShowMultiSubsystem::Tick(float DeltaTime)
{
	if (LoadAndShowStatus != EYMSampleWebpLoadAndShowType::Showing && !WebpTexture)
	{
		return;
	}
	DeltaTime *= 1000;
	for (int32 Index = 0; Index < WebpTimestepMillisecond.Num(); Index++)
	{
		if (Index == 0 && CurrentMillisecond < WebpTimestepMillisecond[Index])
		{
			CurrentMillisecond += DeltaTime;
			WebpShowIndex = Index;
			break;
		}
		if(Index == WebpTimestepMillisecond.Num()-1&&CurrentMillisecond> WebpTimestepMillisecond[Index])
		{
			CurrentMillisecond = 0;
			WebpShowIndex = Index;
			break;
		}
		if (CurrentMillisecond > WebpTimestepMillisecond[Index])
		{
			continue;
		}

		CurrentMillisecond += DeltaTime;
		WebpShowIndex = Index;
		break;
	}

	uint8* textureData = static_cast<uint8*>(WebpTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));
	FMemory::Memcpy(textureData, PictureColors[WebpShowIndex].GetData(), WebpWidth * WebpHeight * 4);
	WebpTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
	WebpTexture->UpdateResource();

}

bool UYMSampleShowMultiSubsystem::IsTickable() const
{
	return !IsTemplate();
}

TStatId UYMSampleShowMultiSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UYMSampleMultiShotWebPSubsystem, STATGROUP_Tickables);
}

void UYMSampleShowMultiSubsystem::LoadWebp(FYMLoadAndShowWebp InLoadAndShowWebpDelegate, FString& InWebpFilePath)
{
	if (!FYMSampleWebPCore::CheckWebpPicturePath(InWebpFilePath)|| LoadAndShowStatus != EYMSampleWebpLoadAndShowType::None)
	{
		InLoadAndShowWebpDelegate.ExecuteIfBound(false, nullptr, -1, -1);
		return;
	}

	LoadAndShowStatus = EYMSampleWebpLoadAndShowType::Loading;

	LoadAndShowWebpDelegate = InLoadAndShowWebpDelegate;

	UYMSampleShowMultiSubsystem* WebpShowMultiSubSystem = this;

	AsyncTask(ENamedThreads::AnyThread, [InWebpFilePath, this]()
		{
			bool bLoadWebp = FYMSampleWebPCore::LoadDynamicWebpPicture(
				InWebpFilePath,
				PictureColors,
				WebpTimestepMillisecond,
				WebpWidth,
				WebpHeight
			);
			AsyncTask(ENamedThreads::GameThread, [bLoadWebp, this]()
				{
					LoadWebpCallShow(bLoadWebp);
				});

		});


}

void UYMSampleShowMultiSubsystem::FreeWebp()
{
	switch (LoadAndShowStatus)
	{
	case EYMSampleWebpLoadAndShowType::None:
		break;
	case EYMSampleWebpLoadAndShowType::Loading://下一帧再执行一次本函数
		AsyncTask(ENamedThreads::GameThread, [this]() {
			FreeWebp();
			});
		break;
	case EYMSampleWebpLoadAndShowType::Showing:
		LoadAndShowStatus = EYMSampleWebpLoadAndShowType::None;
		WebpTexture = nullptr;
		WebpShowIndex = -1;
		CurrentMillisecond = -1;
		WebpHeight = WebpWidth = 0;
		WebpTimestepMillisecond.Empty();
		for (auto& Tmp : PictureColors)
		{
			Tmp.Empty();
		}
		PictureColors.Empty();
		break;
	case EYMSampleWebpLoadAndShowType::Max:
		break;
	default:
		break;
	}
}

void UYMSampleShowMultiSubsystem::LoadWebpCallShow(bool bLoaded)
{
	if (!bLoaded)
	{
		LoadAndShowStatus = EYMSampleWebpLoadAndShowType::None;
		LoadAndShowWebpDelegate.ExecuteIfBound(false, nullptr, -1, -1);
		return;
	}
	LoadAndShowStatus = EYMSampleWebpLoadAndShowType::Showing;
	WebpTexture = UTexture2D::CreateTransient(WebpWidth, WebpHeight, EPixelFormat::PF_R8G8B8A8);

	uint8* textureData = static_cast<uint8*>(WebpTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

	FMemory::Memcpy(textureData, PictureColors[0].GetData(), WebpWidth * WebpHeight * 4);
	WebpShowIndex = 0;
	CurrentMillisecond = 0;
	WebpTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
	WebpTexture->UpdateResource();
	LoadAndShowWebpDelegate.ExecuteIfBound(true, WebpTexture, WebpWidth, WebpHeight);
}
