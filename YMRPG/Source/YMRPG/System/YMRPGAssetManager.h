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
};