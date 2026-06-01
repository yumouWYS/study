#pragma once

#include "CoreMinimal.h"
#include "YMRPGItem.h"
#include "YMRPGEquipment.generated.h"

class UGameplayEffect;

UCLASS()
class YMRPG_API UYMRPGEquipment : public UYMRPGItem
{
	GENERATED_BODY()
public:
	UYMRPGEquipment();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	TSubclassOf<UGameplayEffect> GameplayEffectClass;
};