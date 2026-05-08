#pragma once

#include "Abilities/GameplayAbility.h"

#include "YMRPGGameplayAbility.generated.h"

class UYMRPGAbilitySystemComponent;
class AYMRPGCharacterBase;
class AYMRPGPlayerController;

/*
 * UYMRPGGameplayAbility
 * 
 *  the base gameplay ability class used by this project.	
 */
UCLASS(Abstract, Meta = (ShortTooltip = "Base Gameplay Ability"))
class YMRPG_API UYMRPGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UYMRPGGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};