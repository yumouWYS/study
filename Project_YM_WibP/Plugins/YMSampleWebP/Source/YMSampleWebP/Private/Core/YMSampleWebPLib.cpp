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
