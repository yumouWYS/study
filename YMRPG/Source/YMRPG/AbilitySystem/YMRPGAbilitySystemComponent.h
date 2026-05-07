#pragma once

#include "AbilitySystemComponent.h"

#include "YMRPGAbilitySystemComponent.generated.h"

UCLASS()
class YMRPG_API UYMRPGAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:

	UYMRPGAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};