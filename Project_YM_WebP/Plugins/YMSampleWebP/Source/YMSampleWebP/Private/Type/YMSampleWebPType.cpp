#include "YMSampleWebPType.h"

FYMSampleWebpPictureInformation::FYMSampleWebpPictureInformation():
    X0(0),
    X1(0),
    Y0(0),
    Y1(0)
{
}

FYMSampleWebpPictureInformation::FYMSampleWebpPictureInformation(int32 InX0, int32 InY0, int32 InX1, int32 InY1):
    X0(InX0),
    X1(InX1),
    Y0(InY0),
    Y1(InY1)
{
}

FYMSampleWebpPictureInformation::FYMSampleWebpPictureInformation(FYMSampleWebpPictureInformation& InWebpPictureInformation):
    X0(InWebpPictureInformation.X0),
    X1(InWebpPictureInformation.X1),
    Y0(InWebpPictureInformation.Y0),
    Y1(InWebpPictureInformation.Y1)
{
}

int32 FYMSampleWebpPictureInformation::GetPictureWidth()
{
    return X1 - X0 + 1 > 0 ? X1 - X0 + 1 : 0;
}

int32 FYMSampleWebpPictureInformation::GetPictureHeight()
{
    return Y1 - Y0 + 1 > 0 ? Y1 - Y0 + 1 : 0;
}

void FYMSampleWebpPictureInformation::ResetPictureInformation()
{
    X0 = Y0 = X1 = Y1 = 0;
}
