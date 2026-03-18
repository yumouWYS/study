#include "YMSampleWebPSubsystem.h"
#include "YMSampleWebPCore.h"
#include "Async/Async.h"

bool UYMSampleWebPOneShotSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void UYMSampleWebPOneShotSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	SampleWebPOneShotCallBackDelegate.BindUFunction(this, "SampleWebPOneShotCallBackMethod");
}

void UYMSampleWebPOneShotSubsystem::Deinitialize()
{
	SampleWebPOneShotCallBackDelegate.Unbind();

	Super::Deinitialize();
}

void UYMSampleWebPOneShotSubsystem::BeginSampleWebPOneShot(FYMSampleWebPOneShotCallBackBP InSampleWebPOneShotCallBackDelegateBP)
{
	if (bWorking)
	{
		InSampleWebPOneShotCallBackDelegateBP.ExecuteIfBound(false);

		return;
	}

	bWorking = true;

	SampleWebPOneShotCallBackDelegateBP = InSampleWebPOneShotCallBackDelegateBP;

	FlushRenderingCommands();//确保在执行代码前，渲染线程已经处理完所有已经派发的命令

	ScreenHandle = UGameViewportClient::OnScreenshotCaptured().AddUObject(this, &UYMSampleWebPOneShotSubsystem::ScreenShotCallback);//添加截图代理

	FScreenshotRequest::RequestScreenshot(false);//激活截图代理，同时触发UYMSampleWebPOneShotSubsystem::ScreenShotCallback
}

void UYMSampleWebPOneShotSubsystem::SampleWebPOneShotCallBackMethod(bool bGenerateWebp)
{
	bWorking = false;

	SampleWebPOneShotCallBackDelegateBP.ExecuteIfBound(bGenerateWebp);
}

void UYMSampleWebPOneShotSubsystem::ScreenShotCallback(int32 InWidth, int32 InHeight, const TArray<FColor>& InColors)
{
	if (ScreenHandle.IsValid())
	{
		UGameViewportClient::OnScreenshotCaptured().Remove(ScreenHandle);//本函数已经触发，移除截图代理
	}

	AsyncTask(ENamedThreads::AnyThread, [this, InWidth, InHeight, InColors]() {
		FString PictureName = FGuid::NewGuid().ToString() + TEXT("_OneShot.webp");

		FString TargetPath = FPaths::ProjectSavedDir() / PictureName;

		FVector2D PictureSize(InWidth, InHeight);

		bool FinishResult = FYMSampleWebPCore::GenerateStaticWebpPicture(TargetPath, InColors, PictureSize, 100);

		AsyncTask(ENamedThreads::GameThread, [this, FinishResult]() {

			SampleWebPOneShotCallBackDelegate.ExecuteIfBound(FinishResult);
			});
		});
}
