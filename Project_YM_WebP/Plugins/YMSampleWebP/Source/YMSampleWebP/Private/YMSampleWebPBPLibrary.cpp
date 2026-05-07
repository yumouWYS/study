#include "YMSampleWebPBPLibrary.h"
#include "YMSampleWebPLog.h"
#include "YMSampleMultiShotWebPSubsystem.h"
#include "YMSampleShowMultiSubsystem.h"
#include "YMSampleWebPCore.h"

#include "encode.h"
#include "decode.h"
#include "demux.h"
#include "mux.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"


bool UYMSampleWebPBPLibrary::bAuth = false;

bool UYMSampleWebPBPLibrary::GetYMSampleWebPVersion(FString& OutVersionInfo)
{
    if (!bAuth)
    {
        UE_LOG(LogYMSample, Error, TEXT("该插件未授权，无法使用，请联系作者"));
        return false;
    }
    int32 EncoderVersion = WebPGetEncoderVersion();
    int32 DecoderVersion = WebPGetDecoderVersion();
    int32 MuxVersion = WebPGetMuxVersion();
    int32 DemuxVersion = WebPGetDemuxVersion();

    OutVersionInfo = FString::Printf(TEXT("EncoderVersion:[%d] -- DecoderVersion:[%d] -- MuxVersion:[%d] -- DecoderVersion:[%d]"), EncoderVersion, DecoderVersion, MuxVersion, DemuxVersion);
    return true;
}

void UYMSampleWebPBPLibrary::SetAuth()
{
    bAuth = true;
}

void UYMSampleWebPBPLibrary::BeginRecord(
    UObject* WorldContextObject,
    FString InGeneratedWebpPicturesPath,
    FYMSampleWebpPictureInformation InWebpPictureInformation,
    bool& bBegin
)
{
    bBegin = false;
    if (!WorldContextObject)
    {
        return;
    }
    if (!WorldContextObject->GetWorld())
    {
        return;
    }

    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);

    UYMSampleMultiShotWebPSubsystem* YMWebpMultiShotSubsystem = GameInstance->GetSubsystem<UYMSampleMultiShotWebPSubsystem>();

    TSharedPtr<FYMSampleWebpPictureInformation> WebpPictureInfomationPtr = MakeShareable(new FYMSampleWebpPictureInformation(InWebpPictureInformation));
    
    bBegin = YMWebpMultiShotSubsystem->BeginRecord(InGeneratedWebpPicturesPath, WebpPictureInfomationPtr);
}

void UYMSampleWebPBPLibrary::BeginRecordFullViewport(
    UObject* WorldContextObject,
    FString InGeneratedWebpPicturesPath,
    bool& bBegin
)
{
    bBegin = false;
    if (!WorldContextObject)
    {
        return;
    }
    if (!WorldContextObject->GetWorld())
    {
        return;
    }

    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);

    UYMSampleMultiShotWebPSubsystem* YMWebpMultiShotSubsystem = GameInstance->GetSubsystem<UYMSampleMultiShotWebPSubsystem>();

    FVector2D ViewportSize;

    if (FYMSampleWebPCore::GetViewportSize(WorldContextObject, ViewportSize))
    {
        TSharedPtr<FYMSampleWebpPictureInformation> WebpPictureInfomationPtr = MakeShareable(new FYMSampleWebpPictureInformation(0,0,ViewportSize.X-1,ViewportSize.Y-1));

        bBegin = YMWebpMultiShotSubsystem->BeginRecord(InGeneratedWebpPicturesPath, WebpPictureInfomationPtr);
    }
}

void UYMSampleWebPBPLibrary::EndRecord(
    UObject* WorldContextObject,
    FYMWebpFinishGenerateWebp InfinishWebpBPDelegete
)
{
    if (!WorldContextObject)
    {
        return;
    }
    if (!WorldContextObject->GetWorld())
    {
        return;
    }

    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
    UYMSampleMultiShotWebPSubsystem* YMWebpMultiShotSubsystem = GameInstance->GetSubsystem<UYMSampleMultiShotWebPSubsystem>();

    YMWebpMultiShotSubsystem->EndRecord(InfinishWebpBPDelegete);
}

void UYMSampleWebPBPLibrary::LoadWebp(UObject* WorldContextObject, FYMLoadAndShowWebp InLoadAndShowWebpDelegate, FString InWebpFilePath)
{
    if (!WorldContextObject)
    {
        InLoadAndShowWebpDelegate.ExecuteIfBound(false, nullptr, -1, -1);
        return;
    }
    if (!WorldContextObject->GetWorld())
    {
        InLoadAndShowWebpDelegate.ExecuteIfBound(false, nullptr, -1, -1);
        return;
    }

    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
    UYMSampleShowMultiSubsystem* YMShowMultiSubsystem = GameInstance->GetSubsystem<UYMSampleShowMultiSubsystem>();
    YMShowMultiSubsystem->LoadWebp(InLoadAndShowWebpDelegate, InWebpFilePath);
}

void UYMSampleWebPBPLibrary::FreeWebp(UObject* WorldContextObject)
{
    if (!WorldContextObject)
    {
        return;
    }
    if (!WorldContextObject->GetWorld())
    {
        return;
    }

    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
    UYMSampleShowMultiSubsystem* YMShowMultiSubsystem = GameInstance->GetSubsystem<UYMSampleShowMultiSubsystem>();
    YMShowMultiSubsystem->FreeWebp();
}
