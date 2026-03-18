#include "YMSampleWebPCore.h"
#include "YMSampleWebPLog.h"
#include "YMSampleWebPLib.h"

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
