#include "YMSampleMultiShotWebPSubsystem.h"
#include "YMSampleWebPCore.h"
#include "Misc/Paths.h"
#include "Async/Async.h"

bool UYMSampleMultiShotWebPSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UYMSampleMultiShotWebPSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	SampleWebPMultiShotCallBackDelegate.BindUFunction(this, "SampleMultiShotWebPCallBackMethod");
	WebpPictureInformation = MakeShared<FYMSampleWebpPictureInformation>();
}

void UYMSampleMultiShotWebPSubsystem::Deinitialize()
{
	SampleWebPMultiShotCallBackDelegate.Unbind();
	ResetRecord();

	Super::Deinitialize();
}

void UYMSampleMultiShotWebPSubsystem::Tick(float DeltaTime)
{
	if (ProcessType == EYMSampleWebpProcessType::Recording)
	{
		RecordOneFrame(DeltaTime);
	}
}

bool UYMSampleMultiShotWebPSubsystem::IsTickable() const
{
	return !IsTemplate();
}

TStatId UYMSampleMultiShotWebPSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UYMSampleMultiShotWebPSubsystem, STATGROUP_Tickables);
}

bool UYMSampleMultiShotWebPSubsystem::BeginRecord(
	FString& InGeneratedWebpPicturesPath,
	TSharedPtr<FYMSampleWebpPictureInformation> InWebpPictureInfomation
)
{
	if (!GetWorld())
	{
		return false;
	}
	if (ProcessType != EYMSampleWebpProcessType::None)
	{
		return false;
	}
	if (!FYMSampleWebPCore::CheckWebpPictrueSize(GetWorld(), InWebpPictureInfomation))
	{
		return false;
	}
	ResetRecord();
	ProcessType = EYMSampleWebpProcessType::Recording;
	GeneratedWebpPicturesPath = InGeneratedWebpPicturesPath;
	WebpPictureInformation = InWebpPictureInfomation;
	FlushRenderingCommands();

	ScreenHandle = UGameViewportClient::OnScreenshotCaptured().AddUObject(this, &UYMSampleMultiShotWebPSubsystem::ScreenShotCallback);

	return true;
}

void UYMSampleMultiShotWebPSubsystem::EndRecord(FYMWebpFinishGenerateWebp& InFinishWebpBPDelegate)
{
	if (ProcessType != EYMSampleWebpProcessType::Recording)
	{
		InFinishWebpBPDelegate.ExecuteIfBound(false);
		return;
	}
	if (ScreenHandle.IsValid())
	{
		UGameViewportClient::OnScreenshotCaptured().Remove(ScreenHandle);
	}

	ProcessType = EYMSampleWebpProcessType::Generating;

	FinishWebpBPDelegate = InFinishWebpBPDelegate;

	AsyncTask(ENamedThreads::AnyThread, [&]()
		{
			FPlatformProcess::Sleep(0.2);
			FScopeLock YMLock(&YMWebpMutex);
			
			bool GenerateWebp = FYMSampleWebPCore::GenerateDynamicWebpPicture(
				GeneratedWebpPicturesPath,
				WebpPictureInformation,
				WebpColor,
				WebpTimestepMillisecond
			);
			AsyncTask(ENamedThreads::GameThread, [&, GenerateWebp]()
				{
					SampleWebPMultiShotCallBackDelegate.ExecuteIfBound(GenerateWebp);
				});
		});
}

void UYMSampleMultiShotWebPSubsystem::RecordOneFrame(float DeltaTime)
{
	FScreenshotRequest::RequestScreenshot(false);
	if (WebpTimestepMillisecond.Num() == 0)
	{
		WebpTimestepMillisecond.Add(0);
	}
	else
	{
		WebpTimestepMillisecond.Add(DeltaTime * 1000);
	}
}

void UYMSampleMultiShotWebPSubsystem::ResetRecord()
{
	ProcessType = EYMSampleWebpProcessType::None;
	if (ScreenHandle.IsValid())
	{
		UGameViewportClient::OnScreenshotCaptured().Remove(ScreenHandle);
	}

	GeneratedWebpPicturesPath = TEXT("");
	WebpPictureInformation->ResetPictureInformation();
	WebpColor.Empty();
	WebpTimestepMillisecond.Empty();
}

void UYMSampleMultiShotWebPSubsystem::ScreenShotCallback(int32 InWidth, int32 InHeight, const TArray<FColor>& InColors)
{
	AsyncTask(ENamedThreads::AnyThread, [&, InWidth, InHeight, InColors]() 
		{
			FScopeLock YMLock(&YMWebpMutex);
			TArray<FColor> OutColors;

			for (int32 IndexY = 1; IndexY <= InHeight; IndexY++)
			{
				for (int32 IndexX = 1; IndexX <= InWidth; IndexX++)
				{
					int32 IndexXY = (IndexY - 1) * InWidth + IndexX;
					int32 IndexArray = IndexXY - 1;
					bool bIndexX = (IndexX >= WebpPictureInformation->X0 + 1) && (IndexX <= WebpPictureInformation->X1 + 1);
					bool bIndexY = (IndexY >= WebpPictureInformation->Y0 + 1) && (IndexY <= WebpPictureInformation->Y1 + 1);
					if (bIndexX && bIndexY)
					{
						OutColors.Add(InColors[IndexArray]);
					}
				}
			}

			if (OutColors.Num() == (WebpPictureInformation->X1 - WebpPictureInformation->X0 + 1) * (WebpPictureInformation->Y1 - WebpPictureInformation->Y0 + 1))
			{
				WebpColor.Add(OutColors);
			}

	});
}

void UYMSampleMultiShotWebPSubsystem::SampleMultiShotWebPCallBackMethod(bool bGeneratedWebp)
{
	ResetRecord();
	FinishWebpBPDelegate.ExecuteIfBound(bGeneratedWebp);
}
