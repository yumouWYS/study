#include "YMSampleWebPLib.h"

#if PLATFORM_WINDOWS

#include "encode.h"
#include "decode.h"
#include "mux.h"
#include "demux.h"

#endif

struct FYMSampleWebPCore {

};

bool FYMSampleWebPLib::GenerateWebpByRGBA(
	const char* InWebpSavePath,
	const unsigned char* InRGBAData,
	int InWidch,
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






#endif


	return false;
}
