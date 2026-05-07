#include "YMRPGCharacterBase.h"	
#include "YMRPGAbilitySystemComponent.h"
#include "YMRPGPlayerController.h"
#include "YMRPGPlayerState.h"

AYMRPGCharacterBase::AYMRPGCharacterBase(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	AbilityComponent = CreateDefaultSubobject<UYMRPGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilityComponent->SetIsReplicated(true);
	AbilityComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);


	NetUpdateFrequency = 100.f;
}

AYMRPGPlayerController* AYMRPGCharacterBase::GetPlayerController() const 
{
	return CastChecked<AYMRPGPlayerController>(Controller, ECastCheckedType::NullAllowed);
}

AYMRPGPlayerState* AYMRPGCharacterBase::GetPlayerState() const
{
	return CastChecked<AYMRPGPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);

}

UAbilitySystemComponent* AYMRPGCharacterBase::GetAbilitySystemComponent() const
{
	return AbilityComponent;
}

void AYMRPGCharacterBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if(const UYMRPGAbilitySystemComponent* YMRPGASC = GetYMRPGAbilitySystemComponent())
	{
		YMRPGASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool AYMRPGCharacterBase::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UYMRPGAbilitySystemComponent* YMRPGASC = GetYMRPGAbilitySystemComponent())
	{
		return YMRPGASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool AYMRPGCharacterBase::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UYMRPGAbilitySystemComponent* YMRPGASC = GetYMRPGAbilitySystemComponent())
	{
        return YMRPGASC->HasAllMatchingGameplayTags(TagContainer);
	}
	return false;
}

bool AYMRPGCharacterBase::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UYMRPGAbilitySystemComponent* YMRPGASC = GetYMRPGAbilitySystemComponent())
	{
        return YMRPGASC->HasAnyMatchingGameplayTags(TagContainer);
	}
	return false;
}
