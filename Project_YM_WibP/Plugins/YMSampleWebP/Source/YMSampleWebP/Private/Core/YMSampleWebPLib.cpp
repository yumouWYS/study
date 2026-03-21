#include "YMSampleWebPLib.h"

#if PLATFORM_WINDOWS

#include "encode.h"
#include "decode.h"
#include "mux.h"
#include "demux.h"

#endif

bool FYMSampleWebPLib::GenerateWebpByRGBA(
	const char* InWebpSavePath,
	const unsigned char* InRGBAData,
	int InWidth,
	int InHeight,
	float InQualityFactor
)
{

#if PLATFORM_WINDOWS
	if (InQualityFactor < 0)
	{
		InQualityFactor = 0;
	}
	if (InQualityFactor > 100)
	{
		InQualityFactor = 100;
	}

	uint8_t* Output = nullptr;

	size_t Size = WebPEncodeBGRA(InRGBAData, InWidth, InHeight, InWidth * 4, InQualityFactor, &Output);

	if (Size == 0)
	{
		return false;
	}

	FILE* FDes = fopen(InWebpSavePath, "wb");

	if (FDes)
	{
		fwrite(Output, Size, 1, FDes);
		fclose(FDes);
	}
	else
	{
		WebPFree(Output);
		return false;
	}

	WebPFree(Output);
	return true;

#else

	return false;
#endif
}

bool FYMSampleWebPLib::GenerateDynamicWebpByRGBA(
	const char* InWebpSavePath,
	std::vector<const unsigned char*>& InRGBADatas,
	std::vector<int> InTimestamps_ms,
	int InWidth,
	int InHeight,
	float InQualityFactor
)
{

#if PLATFORM_WINDOWS

	if (InQualityFactor < 0)
	{
		InQualityFactor = 0;
	}
	if (InQualityFactor > 100)
	{
		InQualityFactor = 100;
	}

	if (InRGBADatas.size() != InTimestamps_ms.size())
	{
		return false;
	}

	WebPConfig WebpPictureConfig;

	if (!WebPConfigInit(&WebpPictureConfig))
	{
		return false;
	}

	WebPAnimEncoderOptions EncodeOptions;

	if (!WebPAnimEncoderOptionsInit(&EncodeOptions))
	{
		return false;
	}

	EncodeOptions.kmin = 0;
	EncodeOptions.kmax = 1;

	WebpPictureConfig.lossless = 0;
	WebpPictureConfig.method = 4;
	WebpPictureConfig.quality = InQualityFactor;

	WebPAnimEncoder* Encoder = WebPAnimEncoderNew(InWidth, InHeight, &EncodeOptions);


	float TimePoint = 0.f;

	auto DataItem = InRGBADatas.begin();
	auto TimeStepItem = InTimestamps_ms.begin();

	for (; DataItem != InRGBADatas.end(); ++DataItem, ++TimeStepItem)
	{
		WebPPicture WebpOneFramePicture;
		WebPPictureInit(&WebpOneFramePicture);

		WebpOneFramePicture.use_argb = 1;
		WebpOneFramePicture.width = InWidth;
		WebpOneFramePicture.height = InHeight;
		WebpOneFramePicture.argb_stride = InWidth * 4;

		WebPPictureImportRGBA(&WebpOneFramePicture, *DataItem, InWidth * 4);

		TimePoint += *TimeStepItem;

		WebPAnimEncoderAdd(Encoder, &WebpOneFramePicture, TimePoint, &WebpPictureConfig);
		WebPPictureFree(&WebpOneFramePicture);
	}

	WebPAnimEncoderAdd(Encoder, NULL, TimePoint, NULL);

	WebPData WebpPictureData;
	WebPAnimEncoderAssemble(Encoder, &WebpPictureData);
	WebPAnimEncoderDelete(Encoder);

	FILE* FDes = fopen(InWebpSavePath, "wb");

	if (FDes)
	{
		fwrite(WebpPictureData.bytes , WebpPictureData.size, 1, FDes);
		fclose(FDes);

		WebPDataClear(&WebpPictureData);
		return true;
	}
	else
	{
		WebPDataClear(&WebpPictureData);
		return false;
	}
#else

	return false;
#endif
}
