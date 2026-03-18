// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class YMSAMPLEWEBP_API FYMSampleWebPCore {

public:

	static bool GenerateStaticWebpPicture(const FString& InPicturePath,
		const TArray<FColor>& InPivtureColors,
		const FVector2D& InpictureSize,
		int32 InQualityFactor = 100
	);

public:
	static bool CheckWebpPicturePath(const FString& InPicturePath);

};