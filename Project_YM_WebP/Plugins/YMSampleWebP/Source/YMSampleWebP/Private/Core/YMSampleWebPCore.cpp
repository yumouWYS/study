#include "YMSampleWebPCore.h"
#include "YMSampleWebPLog.h"
#include "YMSampleWebPLib.h"

#include "Misc/Paths.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "Slate/SceneViewport.h"

#include <vector>

bool FYMSampleWebPCore::GenerateStaticWebpPicture(
	const FString& InPicturePath,
	const TArray<FColor>& InPictureColors,
	const FVector2D& InpictureSize,
	int32 InQualityFactor
)
{
	if (!CheckWebpPicturePath(InPicturePath))
	{
		return false;
	}

	if (InPictureColors.Num() != InpictureSize.X * InpictureSize.Y)
	{
		UE_LOG(LogYMSample, Warning, TEXT("[%s] :Wrong Color Num"), *FString(__FUNCTION__));
		return false;
	}

	const char* OutPicturePath = TCHAR_TO_ANSI(*InPicturePath);

	unsigned char* Data = new unsigned char[InPictureColors.Num() * 4 + 1];

	memset(Data, 0, InPictureColors.Num() * 4 + 1);

	int i = 0;

	for (auto& Tmp : InPictureColors)
	{
		Data[i] = Tmp.B;
		i++;
		Data[i] = Tmp.G;
		i++;
		Data[i] = Tmp.R;
		i++;
		Data[i] = Tmp.A;
		i++;
	}

	InQualityFactor = FMath::Clamp(InQualityFactor, 0, 100);

	bool Result = FYMSampleWebPLib::GenerateWebpByRGBA(OutPicturePath, Data, InpictureSize.X, InpictureSize.Y, InQualityFactor);

	delete[] Data;

	Data = nullptr;

	return Result;
}

bool FYMSampleWebPCore::GenerateDynamicWebpPicture(
	const FString& InPicturePath,
	const TSharedPtr<FYMSampleWebpPictureInformation> InWebpPictureInformation,
	const TArray<TArray<FColor>>& InPicturesColors,
	const TArray<int32>& WebpTimestepMillisecond,
	int32 InQualityFactor
)
{
	if (!CheckWebpPicturePath(InPicturePath))
	{
		return false;
	}
	for (auto& InPictureColors : InPicturesColors)
	{
		if (InPictureColors.Num() != InWebpPictureInformation->GetPictureWidth() * InWebpPictureInformation->GetPictureHeight())
		{
			UE_LOG(LogYMSample, Warning, TEXT("[%s] :Wrong Color Num"), *FString(__FUNCTION__));
			return false;
		}
	}
	if (InPicturesColors.Num() != WebpTimestepMillisecond.Num())
	{
		return false;
	}

	const char* OutPicturePath = TCHAR_TO_ANSI(*InPicturePath);

	std::vector<const unsigned char*> RGBADatas;
	std::vector<int> Timestamps_ms;
	for (auto& InPictureColors : InPicturesColors)
	{
		unsigned char* Data = new unsigned char[InPictureColors.Num() * 4 + 1];

		memset(Data, 0, InPictureColors.Num() * 4 + 1);

		int i = 0;

		for (auto& Tmp : InPictureColors)
		{
			Data[i] = Tmp.R;
			i++;
			Data[i] = Tmp.G;
			i++;
			Data[i] = Tmp.B;
			i++;
			Data[i] = Tmp.A;
			i++;
		}

		RGBADatas.push_back(Data);
	}

	for (auto& InTimestamps_ms : WebpTimestepMillisecond)
	{
		Timestamps_ms.push_back(InTimestamps_ms);
	}

	InQualityFactor = FMath::Clamp(InQualityFactor, 0, 100);

	bool Result = FYMSampleWebPLib::GenerateDynamicWebpByRGBA(
		OutPicturePath,
		RGBADatas,
		Timestamps_ms,
		InWebpPictureInformation->GetPictureWidth(),
		InWebpPictureInformation->GetPictureHeight(),
		InQualityFactor
	);

	for (auto& Data : RGBADatas)
	{
		delete[] Data;
		Data = nullptr;
	}
	
	return Result;
}

bool FYMSampleWebPCore::LoadDynamicWebpPicture(
	const FString& InPicturePath,
	TArray<TArray<FColor>>& OutPivtureColors,
	TArray<int32>& OutWebpTimestepMillisecond,
	int32& OutWebpWidth,
	int32& OutWebpHeight
)
{
	if (!CheckWebpPicturePath(InPicturePath))
	{
		return false;
	}

	OutPivtureColors.Empty();
	OutWebpTimestepMillisecond.Empty();

	std::vector<const unsigned char*> OutRGBADatas;
	std::vector<int> OutTimestamps_ms;

	bool bLoadWebp = FYMSampleWebPLib::LoadDynamicWebpPictureByRGBA(
		TCHAR_TO_ANSI(*InPicturePath),
		OutRGBADatas,
		OutTimestamps_ms,
		OutWebpWidth,
		OutWebpHeight
	);

	if (!bLoadWebp)
	{
		return false;
	}

	OutPivtureColors.AddDefaulted(OutRGBADatas.size());
	OutWebpTimestepMillisecond.AddDefaulted(OutTimestamps_ms.size());

	for (int32 WebpIndex = 0; WebpIndex < OutRGBADatas.size(); WebpIndex++)
	{
		TArray<FColor>& OneWebpColor = OutPivtureColors[WebpIndex];
		OneWebpColor.AddDefaulted(OutWebpWidth * OutWebpHeight);

		FMemory::Memcpy(OneWebpColor.GetData(), OutRGBADatas[WebpIndex], OutWebpWidth * OutWebpHeight * 4);
		OutWebpTimestepMillisecond[WebpIndex] = OutTimestamps_ms[WebpIndex];

		free((void*)OutRGBADatas[WebpIndex]);
	}

	return true;
}

bool FYMSampleWebPCore::CheckWebpPicturePath(const FString& InPicturePath)
{
	FString Extension = FPaths::GetExtension(InPicturePath);
	FString Directory = FPaths::GetPath(InPicturePath);

	if (Extension.Equals(TEXT("webp")) && FPaths::DirectoryExists(Directory))
	{
		return true;
	}

	UE_LOG(LogYMSample, Warning, TEXT("FYMSampleWebPCore::CheckWebpPicturePath Wrong Path"));
	return false;
}

bool FYMSampleWebPCore::GetViewportSize(UObject* WorldContextObject, FVector2D& OutViewportSize)
{
	if (!WorldContextObject || !WorldContextObject->GetWorld())
	{
		return false;
	}
	if (UGameViewportClient* InViewportClient = WorldContextObject->GetWorld()->GetGameViewport())
	{
		if (FSceneViewport* InViewprot = InViewportClient->GetGameViewport())
		{
			OutViewportSize = FVector2D::ZeroVector;
			InViewportClient->GetViewportSize(OutViewportSize);

			return true;
		}
	}

	return false;
}

bool FYMSampleWebPCore::CheckWebpPictrueSize(UObject* WorldContextObject, TSharedPtr<FYMSampleWebpPictureInformation> InWebpPictureInfomation)
{
	if (!WorldContextObject || !WorldContextObject->GetWorld())
	{
		return false;
	}
	FVector2D ViewportSize;
	if (GetViewportSize(WorldContextObject, ViewportSize))
	{
		bool bX0 = InWebpPictureInfomation->X0>=0&& InWebpPictureInfomation->X0< InWebpPictureInfomation->X1;
		bool bX1 = InWebpPictureInfomation->X1 <= ViewportSize.X - 1;

		bool bY0 = InWebpPictureInfomation->Y0 >= 0 && InWebpPictureInfomation->Y0 < InWebpPictureInfomation->Y1;
		bool bY1 = InWebpPictureInfomation->Y1 <= ViewportSize.Y - 1;

		return bX0 && bX1 && bY0 && bY1;
	}

	UE_LOG(LogYMSample, Warning, TEXT("Get Viewport Size Error!"));
	return false;
}
