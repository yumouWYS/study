#include "YMRPGAssetManager.h"
#include "AbilitySystemGlobals.h"

UYMRPGAssetManager::UYMRPGAssetManager()
{
}

void UYMRPGAssetManager::StartInitialLoading()
{
    Super::StartInitialLoading();

    UAbilitySystemGlobals::Get().InitGlobalData();
}
