// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "YMSampleWebPType.h"

class YMSAMPLEWEBP_API FYMSampleWebPCore {

public:

	static bool GenerateStaticWebpPicture(const FString& InPicturePath,
		const TArray<FColor>& InPivtureColors,
		const FVector2D& InpictureSize,
		int32 InQualityFactor = 100
	);
	
	static bool GenerateDynamicWebpPicture(const FString& InPicturePath,
		const TSharedPtr<FYMSampleWebpPictureInformation> InWebpPictureInformation,
		const TArray<TArray<FColor>>& InPivtureColors,
		const TArray<int32>& WebpTimestepMillisecond,
		int32 InQualityFactor = 100
	);

public:
	static bool CheckWebpPicturePath(const FString& InPicturePath);

	static bool GetViewportSize(UObject* WorldContextObject, FVector2D& OutViewprotSize);

	static bool CheckWebpPictrueSize(UObject* WorldContextObject, TSharedPtr<FYMSampleWebpPictureInformation> InWebpPictureInfomation);

};