// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"



struct FYMSampleWebPLib {

	friend class FYMSampleWebPCore;

protected:

	static bool GenerateWebpByRGBA(
		const char* InWebpSavePath,
		const unsigned char* InRGBAData,
		int InWidch,
		int InHeight,
		float InQualityFactor = 100
	);
};