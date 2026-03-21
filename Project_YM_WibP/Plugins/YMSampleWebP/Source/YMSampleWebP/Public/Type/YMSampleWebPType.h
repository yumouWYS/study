// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "YMSampleWebPType.generated.h"


DECLARE_DYNAMIC_DELEGATE_OneParam(FYMWebpFinishGenerateWebp, bool, bFinishGanerate);


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