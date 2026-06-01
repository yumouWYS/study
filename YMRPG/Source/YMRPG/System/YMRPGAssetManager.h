#pragma once

#include "Engine/AssetManager.h"
#include "Templates/SubClassOf.h"
#include "YMRPGAssetManager.generated.h"

UCLASS(Config = Game)
class UYMRPGAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
    	/** Asset Subsystem */
	UYMRPGAssetManager();

	virtual void StartInitialLoading() override;

	static const FPrimaryAssetType PotionItemType;
	static const FPrimaryAssetType TokenItemType;
	static const FPrimaryAssetType EquipmentItemType;

	class UYMRPGItem* ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning = true);
};