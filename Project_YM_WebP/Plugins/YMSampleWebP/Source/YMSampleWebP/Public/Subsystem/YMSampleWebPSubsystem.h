// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "YMSampleWebPSubsystem.generated.h"

//
DECLARE_DELEGATE_OneParam(FYMSampleWebPOneShotCallBack, bool);

DECLARE_DYNAMIC_DELEGATE_OneParam(FYMSampleWebPOneShotCallBackBP, bool, bGenerateWebp);

UCLASS()
class YMSAMPLEWEBP_API UYMSampleWebPOneShotSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

public:
	UFUNCTION(BlueprintCallable, Category= "YMSampleWebp|Subsystem")
	void BeginSampleWebPOneShot(FYMSampleWebPOneShotCallBackBP InSampleWebPOneShotCallBackDelegateBP);

protected:
	FYMSampleWebPOneShotCallBack SampleWebPOneShotCallBackDelegate;

	FYMSampleWebPOneShotCallBackBP SampleWebPOneShotCallBackDelegateBP;

	FDelegateHandle ScreenHandle;

	bool bWorking;

	UFUNCTION()
	void SampleWebPOneShotCallBackMethod(bool bGenerateWebp);

	UFUNCTION()
	void ScreenShotCallback(int32 InWidth, int32 InHeight, const TArray<FColor>& InColors);

};