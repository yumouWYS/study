// Copyright Epic Games, Inc. All Right Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "YMSampleWebPBPLibrary.generated.h"


UCLASS()
class UYMSampleWebPBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (DisplayName = "GetYMSampleWebPVersion", Keywords = "Get YM Sample WebP Version"), Category = "YMSampleWebP")
	static bool GetYMSampleWebPVersion(FString& OutVersionInfo);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SetAuth", Keywords = "Set Auth"), Category = "YMSampleWebP")
	static void SetAuth();

protected:
	static bool bAuth;
};