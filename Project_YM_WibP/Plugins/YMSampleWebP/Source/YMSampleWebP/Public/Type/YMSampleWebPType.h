// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Engine/Texture2D.h"
#include "CoreMinimal.h"
#include "YMSampleWebPType.generated.h"


DECLARE_DYNAMIC_DELEGATE_OneParam(FYMWebpFinishGenerateWebp, bool, bFinishGanerate);


DECLARE_DYNAMIC_DELEGATE_FourParams(FYMLoadAndShowWebp, bool, bLoad, UTexture2D*, OutWebpPicture, int32, WebpTidth, int32, WebpHeight);


USTRUCT(BlueprintType)
struct YMSAMPLEWEBP_API FYMSampleWebpPictureInformation 
{
	GENERATED_USTRUCT_BODY()

public:
	FYMSampleWebpPictureInformation();
	FYMSampleWebpPictureInformation(int32 InX0, int32 InY0, int32 InX1, int32 InY1);
	FYMSampleWebpPictureInformation(FYMSampleWebpPictureInformation& InWebpPictureInformation);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, CateGory = "YMWebp")
	int32 X0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YMWebp")
	int32 Y0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YMWebp")
	int32 X1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YMWebp")
	int32 Y1;

	int32 GetPictureWidth();

	int32 GetPictureHeight();

	void ResetPictureInformation();
};


UENUM()
enum class EYMSampleWebpProcessType :uint8
{
	None,
	Recording,
	Generating,
	Max
};

UENUM()
enum class EYMSampleWebpLoadAndShowType :uint8
{
	None,
	Loading,
	Showing,
	Max
};