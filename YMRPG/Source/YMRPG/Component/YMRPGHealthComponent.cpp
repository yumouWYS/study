#include "YMRPGHealthComponent.h"

#include "Net/UnrealNetwork.h"

#include "YMRPG.h"
#include "YMRPGGameplayTag.h"

#include "YMRPGAbilitySystemComponent.h"
#include "YMRPGCharacterAttributeSet.h"
#include "YMRPGGameplayAbility.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_Damage, "Gameplay.Damage");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageImmunity, "Gameplay.DamageImmunity");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageSelfDestruct, "Gameplay.Damage.SelfDestruct");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageFellOutOfWorld, "Gameplay.Damage.FellOutOfWorld");
UE_DEFINE_GAMEPLAY_TAG(TAG_YMRPG_Damage_Message, "YMRPG.Damage.Message");

UYMRPGHealthComponent::UYMRPGHealthComponent(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
	DeathState = EYMRPGDeathState::NotDead;
}

void UYMRPGHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UYMRPGHealthComponent, DeathState);
}



void UYMRPGHealthComponent::InitializeWithAbilitySystem(UYMRPGAbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		UE_LOG(LogYMRPG, Error, TEXT("YMRPGHealthComponent: Health component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogYMRPG, Error, TEXT("YMRPGHealthComponent: Cannot initialize health component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}

	HealthSet = AbilitySystemComponent->GetSet<UYMRPGCharacterAttributeSet>();

	if (!HealthSet)
	{
		UE_LOG(LogYMRPG, Error, TEXT("YMRPGHealthComponent: Cannot initialize health component for owner [%s] with NULL attribute set."), *GetNameSafe(Owner));
		return;
	}

	HealthSet->OnHealthChanged.AddUObject(this, &UYMRPGHealthComponent::HandleHealthChanged);
	HealthSet->OnMaxHealthChanged.AddUObject(this, &UYMRPGHealthComponent::HandleMaxHealthChanged);
	HealthSet->OnOutOfHealth.AddUObject(this, &UYMRPGHealthComponent::HandleOutOfHealth);
	HealthSet->OnManaChanged.AddUObject(this, &UYMRPGHealthComponent::HandleManaChanged);
	HealthSet->OnMaxManaChanged.AddUObject(this, &UYMRPGHealthComponent::HandleMaxManaChanged);

	AbilitySystemComponent->SetNumericAttributeBase(UYMRPGCharacterAttributeSet::GetHealthAttribute(), HealthSet->GetMaxHealth());
	AbilitySystemComponent->SetNumericAttributeBase(UYMRPGCharacterAttributeSet::GetManaAttribute(), HealthSet->GetMaxMana());

	ClearGameplayTags();

	OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	OnMaxHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	OnManaChanged.Broadcast(this, HealthSet->GetMana(), HealthSet->GetMana(), nullptr);
	OnMaxManaChanged.Broadcast(this, HealthSet->GetMana(), HealthSet->GetMana(), nullptr);
}

void UYMRPGHealthComponent::UninitializeFromAbilitySystem()
{
	ClearGameplayTags();

	if (HealthSet)
	{
		HealthSet->OnHealthChanged.RemoveAll(this);
		HealthSet->OnMaxHealthChanged.RemoveAll(this);
		HealthSet->OnOutOfHealth.RemoveAll(this);
		HealthSet->OnManaChanged.RemoveAll(this);
		HealthSet->OnMaxManaChanged.RemoveAll(this);
	}
	HealthSet = nullptr;
	AbilitySystemComponent = nullptr;
}

float UYMRPGHealthComponent::GetHealth() const
{
	return HealthSet?HealthSet->GetHealth():0.f;
}

float UYMRPGHealthComponent::GetMaxHealth() const
{
	return HealthSet ? HealthSet->GetMaxHealth() : 0.f;
}

float UYMRPGHealthComponent::GetMana() const
{
	return HealthSet ? HealthSet->GetMana() : 0.f;
}

float UYMRPGHealthComponent::GetMaxMana() const
{
	return HealthSet ? HealthSet->GetMaxMana() : 0.f;
}

float UYMRPGHealthComponent::GetHealthPercent() const
{
	if(HealthSet)
	{
		const float MaxHealth = HealthSet->GetMaxHealth();
		return MaxHealth > 0.f ? HealthSet->GetHealth() / MaxHealth : 0.f;
	}
	return 0.f;
}

float UYMRPGHealthComponent::GetManaPercent() const
{
	if (HealthSet)
	{
		const float MaxMana = HealthSet->GetMaxMana();
		return MaxMana > 0.f ? HealthSet->GetMana() / MaxMana : 0.f;
	}
	return 0.f;
}

void UYMRPGHealthComponent::StartDeath()
{
	if (DeathState != EYMRPGDeathState::NotDead)
	{
		return;
	}

	DeathState = EYMRPGDeathState::DeathStarted;

	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(YMRPGGameplayTags::Status_Death_Dying, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathStarted.Broadcast(Owner);

	Owner->ForceNetUpdate();
}

void UYMRPGHealthComponent::FinishDeath()
{
	if (DeathState != EYMRPGDeathState::DeathStarted)
	{
		return;
	}

	DeathState = EYMRPGDeathState::DeathFinished;

	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(YMRPGGameplayTags::Status_Death_Dying, 0);
		AbilitySystemComponent->SetLooseGameplayTagCount(YMRPGGameplayTags::Status_Death_Dead, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathFinished.Broadcast(Owner);

	Owner->ForceNetUpdate();
}

void UYMRPGHealthComponent::DamageSelfDestruct(bool bFellOutOfWorld)
{

}

void UYMRPGHealthComponent::OnUnregister()
{	
	UninitializeFromAbilitySystem();
	Super::OnUnregister();

}

void UYMRPGHealthComponent::ClearGameplayTags()
{
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(YMRPGGameplayTags::Status_Death_Dying, 0);
		AbilitySystemComponent->SetLooseGameplayTagCount(YMRPGGameplayTags::Status_Death_Dead, 0);
	}
}

void UYMRPGHealthComponent::OnRep_DeathState(EYMRPGDeathState OldValue)
{
	const EYMRPGDeathState NewValue = DeathState;
	DeathState = OldValue;
	if (OldValue > NewValue)
	{
		UE_LOG(LogYMRPG, Error, TEXT("YMRPGHealthComponent: Predict past server death state [%d] -> [%d] for owner [%s]."),(uint8)OldValue, (uint8)NewValue, *GetNameSafe(GetOwner()));
		return;
	}

	if (OldValue == EYMRPGDeathState::NotDead)
	{
		if (NewValue == EYMRPGDeathState::DeathStarted)
		{
			StartDeath();
		}
		else if(NewValue == EYMRPGDeathState::DeathFinished)
		{
			StartDeath();
			FinishDeath();
		}
		else
		{
			UE_LOG(LogYMRPG, Error, TEXT("YMRPGHealthComponent: Predict past server death state [%d] -> [%d] for owner [%s]."), (uint8)OldValue, (uint8)NewValue, *GetNameSafe(GetOwner()));
		}
	}
	else if (OldValue == EYMRPGDeathState::DeathStarted)
	{
		if (NewValue == EYMRPGDeathState::DeathFinished)
		{
			FinishDeath();
		}
		else
		{
			UE_LOG(LogYMRPG, Error, TEXT("YMRPGHealthComponent: Predict past server death state [%d] -> [%d] for owner [%s]."), (uint8)OldValue, (uint8)NewValue, *GetNameSafe(GetOwner()));
		}
	}

	ensureMsgf(DeathState == NewValue, TEXT("YMRPGHealthComponent: Death state [%d] -> [%d] for owner [%s] did not match expected value after prediction."), (uint8)OldValue, (uint8)NewValue, *GetNameSafe(GetOwner()));
}

/*
DECLARE_MULTICAST_DELEGATE_SixParams(
FYMRPGAttributeEvent,
AActor* /*EffectInstigator,
AActor* /*EffectCauser,
	const FGameplayEffectSpec* /*EffectSpec,
	float /*EffectMagnitude,
	float /*OldValue,
	float /*NewValue
	);
*/

void UYMRPGHealthComponent::HandleHealthChanged(
	AActor* Instigator,
	AActor* Causer,
	const FGameplayEffectSpec* Spec,
	float DamageMagnitude,
	float OldValue,
	float NewValue
)  
{
	OnHealthChanged.Broadcast(this, OldValue, NewValue, Instigator);
}

void UYMRPGHealthComponent::HandleMaxHealthChanged(
	AActor* Instigator,
	AActor* Causer,
	const FGameplayEffectSpec* Spec,
	float DamageMagnitude,
	float OldValue,
	float NewValue
)
{
	OnMaxHealthChanged.Broadcast(this, OldValue, NewValue, Instigator);
}

void UYMRPGHealthComponent::HandleOutOfHealth(
	AActor* Instigator,
	AActor* Causer,
	const FGameplayEffectSpec* Spec,
	float DamageMagnitude,
	float OldValue,
	float NewValue
)
{
#if WITH_SERVER_CODE
	if (DeathState == EYMRPGDeathState::NotDead)
	{
		StartDeath();
	}
#endif  // WITH_SERVER_CODE
}

void UYMRPGHealthComponent::HandleManaChanged(
	AActor* Instigator,
	AActor* Causer,
	const FGameplayEffectSpec* Spec, 
	float DamageMagnitude,
	float OldValue,
	float NewValue
)
{
	OnManaChanged.Broadcast(this, OldValue, NewValue, Instigator);
}

void UYMRPGHealthComponent::HandleMaxManaChanged(
	AActor* Instigator,
	AActor* Causer,
	const FGameplayEffectSpec* Spec,
	float DamageMagnitude,
	float OldValue, 
	float NewValue
)
{
	OnMaxManaChanged.Broadcast(this, OldValue, NewValue, Instigator);
}
