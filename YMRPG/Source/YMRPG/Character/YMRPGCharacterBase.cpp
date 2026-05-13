#include "YMRPGCharacterBase.h"	
#include "YMRPGAbilitySystemComponent.h"
#include "YMRPGPlayerController.h"
#include "YMRPGPlayerState.h"
#include "YMRPGGameplayAbility.h"
#include "YMRPGComboComponent.h"

AYMRPGCharacterBase::AYMRPGCharacterBase(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	AbilityComponent = CreateDefaultSubobject<UYMRPGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilityComponent->SetIsReplicated(true);
	AbilityComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);


	ComboComponent = CreateDefaultSubobject<UYMRPGComboComponent>(TEXT("ComboComponent"));
	ComboComponent->SetIsReplicated(false);


	NetUpdateFrequency = 100.f;
}

AYMRPGPlayerController* AYMRPGCharacterBase::GetPlayerController() const 
{
	return CastChecked<AYMRPGPlayerController>(Controller, ECastCheckedType::NullAllowed);
}

AYMRPGPlayerState* AYMRPGCharacterBase::GetPlayerState() const
{
	return CastChecked<AYMRPGPlayerState>(APawn::GetPlayerState(), ECastCheckedType::NullAllowed);
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

void AYMRPGCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (AbilityComponent && GetLocalRole() == ROLE_Authority)
	{
		AbilityComponent->InitAbilityActorInfo(this, this);

		for (auto& TmpAbilityPair : AbilitiesToAdd)
		{
			UYMRPGGameplayAbility* AbilityCDO = TmpAbilityPair.Value->GetDefaultObject<UYMRPGGameplayAbility>();

			FGameplayAbilitySpec AbilitySpec(AbilityCDO, 1);

			AbilitySpec.SourceObject = this;
			AbilitySpec.DynamicAbilityTags.AddTag(TmpAbilityPair.Key);

			const FGameplayAbilitySpecHandle AbilityHandle = AbilityComponent->GiveAbility(AbilitySpec);

			AbilitiesToActive.Add(TmpAbilityPair.Key, AbilityHandle);
		}
	}
}

void AYMRPGCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	Super::EndPlay(EndPlayReason);
}
