// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "YMSampleWebPType.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "YMSampleShowMultiSubsystem.generated.h"

UCLASS()
class YMSAMPLEWEBP_API UYMSampleShowMultiSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

	friend class UYMSampleWebPBPLibrary;

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

public:

	virtual void Tick(float DeltaTime) override;
	
	virtual bool IsTickable() const override;

	virtual TStatId GetStatId()const override;

protected:

	void LoadWebp(FYMLoadAndShowWebp InLoadAndShowWebpDelegate, FString& InWebpFilePath);

	void FreeWebp();

	void LoadWebpCallShow(bool bLoaded);

protected:

	UPROPERTY()
	UTexture2D* WebpTexture = nullptr;

	EYMSampleWebpLoadAndShowType LoadAndShowStatus = EYMSampleWebpLoadAndShowType::None;

	FYMLoadAndShowWebp LoadAndShowWebpDelegate;

	int32 WebpShowIndex = -1;
	int32 CurrentMillisecond = -1;

	TArray<int32> WebpTimestepMillisecond;
	TArray<TArray<FColor>> PictureColors;

	int32 WebpWidth = 0;
	int32 WebpHeight = 0;
};