// Copyright Epic Games, Inc. All Right Reserved.

#pragma once

#include "Coreminimal.h"
#include "YMSampleWebPType.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "YMSampleWebPBPLibrary.generated.h"


UCLASS()
class YMSAMPLEWEBP_API UYMSampleWebPBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetYMSampleWebPVersion", Keywords = "Get YM Sample WebP Version"), Category = "YMSampleWebP")
	static bool GetYMSampleWebPVersion(FString& OutVersionInfo);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetAuth", Keywords = "Set Auth"), Category = "YMSampleWebP")
	static void SetAuth();

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DisplayName = "BeginRecord"), Category = "YMSampleWebP")
	static void BeginRecord(
		UObject* WorldContextObject,
		FString InGeneratedWebpPicturesPath,
		FYMSampleWebpPictureInformation InWebpPictureInformation,
		bool& bBegin
	);


	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DisplayName = "BeginRecordFullViewport"), Category = "YMSampleWebP")
	static void BeginRecordFullViewport(
		UObject* WorldContextObject,
		FString InGeneratedWebpPicturesPath,
		bool& bBegin
	);


	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DisplayName = "EndRecord"), Category = "YMSampleWebP")
	static void EndRecord(
		UObject* WorldContextObject,
		FYMWebpFinishGenerateWebp InfinishWebpBPDelegete
	);


protected:
	static bool bAuth;
};