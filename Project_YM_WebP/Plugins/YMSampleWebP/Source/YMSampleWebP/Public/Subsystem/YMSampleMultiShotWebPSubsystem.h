// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "YMSampleWebPType.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "YMSampleMultiShotWebPSubsystem.generated.h"


DECLARE_DELEGATE_OneParam(FYMSampleMultiShotWebPCallBack, bool);


UCLASS()
class YMSAMPLEWEBP_API UYMSampleMultiShotWebPSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
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

	bool BeginRecord(FString& InGeneratedWebpPicturesPath, TSharedPtr<FYMSampleWebpPictureInformation> InWebpPictureInfomation);

	void EndRecord(FYMWebpFinishGenerateWebp& InFinishWebpBPDelegate);

	void RecordOneFrame(float DeltaTime);

	void ResetRecord();

protected:
	UFUNCTION()
	void ScreenShotCallback(int32 InWidth, int32 InHeight, const TArray<FColor>& InColors);

	UFUNCTION()
	void SampleMultiShotWebPCallBackMethod(bool bGeneratedWebp);

protected:

	EYMSampleWebpProcessType ProcessType = EYMSampleWebpProcessType::None;
	FString GeneratedWebpPicturesPath = FString();
	TSharedPtr<FYMSampleWebpPictureInformation> WebpPictureInformation;

	FDelegateHandle ScreenHandle;

	TArray<TArray<FColor>> WebpColor;
	TArray<int32> WebpTimestepMillisecond;

	FYMWebpFinishGenerateWebp FinishWebpBPDelegate;
	FYMSampleMultiShotWebPCallBack SampleWebPMultiShotCallBackDelegate;

	FCriticalSection YMWebpMutex;
};