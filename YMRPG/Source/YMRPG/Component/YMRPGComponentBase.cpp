#include "YMRPGComponentBase.h"

UYMRPGComponentBase::UYMRPGComponentBase(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}
