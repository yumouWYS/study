#include "YMSampleWebPBPLibrary.h"

#include "encode.h"
#include "decode.h"
#include "demux.h"
#include "mux.h"

#include "YMSampleWebPLog.h"

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
