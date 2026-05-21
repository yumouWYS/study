#include "YMRPGGameplayAbility_Death.h"

#include "YMRPGGameplayAbility.h"
#include "YMRPGAbilitySystemComponent.h"
#include "YMRPGHealthComponent.h"
#include "YMRPGGameplayTag.h"
#include "YMRPG.h"

UYMRPGGameplayAbility_Death::UYMRPGGameplayAbility_Death(const FObjectInitializer& ObjectInitializer)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;


	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
        TriggerData.TriggerTag = YMRPGGameplayTags::GameplayEvent_Death;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}


	bAutoStartDeath = true;
}

void UYMRPGGameplayAbility_Death::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	check(ActorInfo);

	UYMRPGAbilitySystemComponent* ASC = Cast<UYMRPGAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

	FGameplayTagContainer AbilityTypesToIgnore;
	AbilityTypesToIgnore.AddTag(YMRPGGameplayTags::Ability_Behavior_SurvivesDeath);

	ASC->CancelAbilities(nullptr, &AbilityTypesToIgnore, this);

	SetCanBeCanceled(false);

	if (bAutoStartDeath)
	{
		StartDeath();
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UYMRPGGameplayAbility_Death::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	check(ActorInfo);
	FinishDeath();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UYMRPGGameplayAbility_Death::StartDeath()
{
	if (UYMRPGHealthComponent* HealthComponent = UYMRPGHealthComponent::GetHealthComponent(GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == EYMRPGDeathState::NotDead)
		{
			HealthComponent->StartDeath();
		}
	}
}

void UYMRPGGameplayAbility_Death::FinishDeath()
{
	if (UYMRPGHealthComponent* HealthComponent = UYMRPGHealthComponent::GetHealthComponent(GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == EYMRPGDeathState::DeathStarted)
		{
			HealthComponent->FinishDeath();
		}
	}
}