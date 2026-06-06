#pragma once

#include "CoreMinimal.h"
#include "YMRPGItem.h"
#include "YMRPGPotion.generated.h"

class UYMRPGGameplayAbility;

UCLASS()
class YMRPG_API UYMRPGPotion : public UYMRPGItem
{
	GENERATED_BODY()
public:
	UYMRPGPotion();


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "YMRPG|Potion")
	TSubclassOf<UYMRPGGameplayAbility> GrantedAbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "YMRPG|Potion")
	int32 AbilityLevel;
};