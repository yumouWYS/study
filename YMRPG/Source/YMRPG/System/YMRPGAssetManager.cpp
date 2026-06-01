#include "YMRPGAssetManager.h"
#include "AbilitySystemGlobals.h"
#include "YMRPG.h"
#include "YMRPGItem.h"

const FPrimaryAssetType UYMRPGAssetManager::PotionItemType = TEXT("Potion");
const FPrimaryAssetType UYMRPGAssetManager::TokenItemType = TEXT("Token");
const FPrimaryAssetType UYMRPGAssetManager::EquipmentItemType = TEXT("Equipment");

UYMRPGAssetManager::UYMRPGAssetManager()
{
}

void UYMRPGAssetManager::StartInitialLoading()
{
    Super::StartInitialLoading();

    UAbilitySystemGlobals::Get().InitGlobalData();
}

UYMRPGItem* UYMRPGAssetManager::ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning)
{
	FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

	// This does a synchronous load and may hitch
	UYMRPGItem* LoadedItem = Cast<UYMRPGItem>(ItemPath.TryLoad());

	if (bLogWarning && LoadedItem == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load item for identifier %s!"), *PrimaryAssetId.ToString());
	}

	return LoadedItem;
}
