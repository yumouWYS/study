#include "YMRPGCharacterAttributeSet.h"

#include "Net/UnrealNetwork.h"
#include "AttributeSet.h"
#include "GameplayEffectExtension.h"

#include "AbilitySystemComponent.h"
#include "YMRPGAbilitySystemComponent.h"


UYMRPGCharacterAttributeSet::UYMRPGCharacterAttributeSet()
: Health(100.f)
, MaxHealth(100.f)
, Mana(80.f)
, MaxMana(80.f)
, Stamina(50.f)
, MaxStamina(50.f)
, Attack(10.f)
, Defense(2.f)
, Exp(0.f)
, MaxExp(120.f)
, Level(1)
, bOutOfHealth(false)
, MaxHealthBeforeAttributeChange(0.f)
, HealthBeforeAttributeChange(0.f)
{

}



void UYMRPGCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UYMRPGCharacterAttributeSet, Damage, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UYMRPGCharacterAttributeSet, Level, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UYMRPGCharacterAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UYMRPGCharacterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UYMRPGCharacterAttributeSet, Mana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UYMRPGCharacterAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UYMRPGCharacterAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UYMRPGCharacterAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UYMRPGCharacterAttributeSet, Attack, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UYMRPGCharacterAttributeSet, Defense, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UYMRPGCharacterAttributeSet, Exp, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UYMRPGCharacterAttributeSet, MaxExp, COND_None, REPNOTIFY_Always);
}

void UYMRPGCharacterAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UYMRPGCharacterAttributeSet, Damage, OldValue);
}

void UYMRPGCharacterAttributeSet::OnRep_Level(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UYMRPGCharacterAttributeSet, Level, OldValue);
}

void UYMRPGCharacterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UYMRPGCharacterAttributeSet, Health, OldValue);

	const float CurrentHealth = GetHealth();
    const float EstimatedMagnitude = CurrentHealth - OldValue.GetCurrentValue();

    OnHealthChanged.Broadcast(nullptr,nullptr,nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);

    if (!bOutOfHealth && CurrentHealth <= 0.f)
    {
        OnOutOfHealth.Broadcast(nullptr, nullptr, nullptr, 0.f, OldValue.GetCurrentValue(), CurrentHealth);
    }

    bOutOfHealth = CurrentHealth <= 0.f;
}

void UYMRPGCharacterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UYMRPGCharacterAttributeSet, MaxHealth, OldValue);
}

void UYMRPGCharacterAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UYMRPGCharacterAttributeSet, Mana, OldValue);

    const float CurrentMana = GetMana();
    const float EstimatedMagnitude = CurrentMana - OldValue.GetCurrentValue();

    OnManaChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentMana);
}

void UYMRPGCharacterAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
    	GAMEPLAYATTRIBUTE_REPNOTIFY(UYMRPGCharacterAttributeSet, MaxMana, OldValue);
}

void UYMRPGCharacterAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
    	GAMEPLAYATTRIBUTE_REPNOTIFY(UYMRPGCharacterAttributeSet, Stamina, OldValue);
}

void UYMRPGCharacterAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UYMRPGCharacterAttributeSet, MaxStamina, OldValue);
}

void UYMRPGCharacterAttributeSet::OnRep_Attack(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UYMRPGCharacterAttributeSet, Attack, OldValue);
}

void UYMRPGCharacterAttributeSet::OnRep_Defense(const FGameplayAttributeData& OldValue)
{
    	GAMEPLAYATTRIBUTE_REPNOTIFY(UYMRPGCharacterAttributeSet, Defense, OldValue);
}

void UYMRPGCharacterAttributeSet::OnRep_Exp(const FGameplayAttributeData& OldValue)
{
    	GAMEPLAYATTRIBUTE_REPNOTIFY(UYMRPGCharacterAttributeSet, Exp, OldValue);
}

void UYMRPGCharacterAttributeSet::OnRep_MaxExp(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UYMRPGCharacterAttributeSet, MaxExp, OldValue);
}

bool UYMRPGCharacterAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
    Super::PreGameplayEffectExecute(Data);

    HealthBeforeAttributeChange = GetHealth();
    MaxHealthBeforeAttributeChange = GetMaxHealth();

    return true;
}

void UYMRPGCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    const FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();

	AActor* EffectInstigator = Context.GetInstigator();
    AActor* EffectCauser = Context.GetEffectCauser();

    if (Data.EvaluatedData.Attribute == GetDamageAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth() - GetDamage(), 0.f, GetMaxHealth()));
        SetDamage(0.0f);
    }
    else if(Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
    }
    else if(Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
    {
        OnMaxHealthChanged.Broadcast(
            EffectInstigator,
            EffectCauser,
            &Data.EffectSpec,
            Data.EvaluatedData.Magnitude,
            MaxHealthBeforeAttributeChange,
            GetMaxHealth()
        );
    }
    else if (Data.EvaluatedData.Attribute == GetManaAttribute())
    {
        SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));

    }
    else if (Data.EvaluatedData.Attribute == GetMaxManaAttribute())
    {
        OnMaxManaChanged.Broadcast(
            EffectInstigator,
            EffectCauser,
            &Data.EffectSpec,
            Data.EvaluatedData.Magnitude,
            MaxHealthBeforeAttributeChange,
            GetMaxMana()
        );
    }

    if (GetHealth() != HealthBeforeAttributeChange)
    {
        OnHealthChanged.Broadcast(
            EffectInstigator,
            EffectCauser,
            &Data.EffectSpec,
            Data.EvaluatedData.Magnitude,
            HealthBeforeAttributeChange,
            GetHealth()
		);
    }

    if (GetHealth() <= 0.f && !bOutOfHealth)
    {
        OnOutOfHealth.Broadcast(
            EffectInstigator,
            EffectCauser,
            &Data.EffectSpec,
            Data.EvaluatedData.Magnitude,
            HealthBeforeAttributeChange,
            GetHealth()
        );
    }

    bOutOfHealth = GetHealth() <= 0.f;
}

void UYMRPGCharacterAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
    return Super::PreAttributeBaseChange(Attribute, NewValue);
}

void UYMRPGCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    return Super::PreAttributeChange(Attribute, NewValue);
}

void UYMRPGCharacterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
    Super::PostAttributeChange(Attribute, OldValue, NewValue);

    if (Attribute == GetMaxHealthAttribute())
    {
        UYMRPGAbilitySystemComponent* YMRPGASC = GetYMRPGAbilitySystemComponent();
        check(YMRPGASC);

        YMRPGASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
    }

    if (bOutOfHealth && (GetHealth() > 0.f))
    {
		bOutOfHealth = false;
    }
}

void UYMRPGCharacterAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
    }
    else if( Attribute == GetMaxHealthAttribute())
    {
        NewValue = FMath::Max(NewValue, 1.f);
    }
}
