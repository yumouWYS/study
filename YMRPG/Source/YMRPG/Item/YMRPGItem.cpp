#include "YMRPGItem.h"

bool UYMRPGItem::IsConsumable() const
{
	if(MaxCount<=0)
	{
		return false;
	}
	return true;
}

FString UYMRPGItem::GetIdentifierString() const
{
	return GetPrimaryAssetId().ToString();
}

FPrimaryAssetId UYMRPGItem::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(ItemType, GetFName());
}
