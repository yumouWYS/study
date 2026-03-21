// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "vector"



struct FYMSampleWebPLib {

	friend class FYMSampleWebPCore;

protected:

	static bool GenerateWebpByRGBA(
		const char* InWebpSavePath,
		const unsigned char* InRGBAData,
		int InWidth,
		int InHeight,
		float InQualityFactor = 100
	);

	static bool GenerateDynamicWebpByRGBA(
		const char* InWebpSavePath,
		std::vector<const unsigned char*>& InRGBAData,
		std::vector<int> InTimestamps_ms,
		int InWidth,
		int InHeight,
		float InQualityFactor = 100
	);
};