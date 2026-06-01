#include "YMRPGEquipment.h"

#include "YMRPGAssetManager.h"

UYMRPGEquipment::UYMRPGEquipment()
{
	ItemType = UYMRPGAssetManager::EquipmentItemType;
	MaxCount = 1;
}
