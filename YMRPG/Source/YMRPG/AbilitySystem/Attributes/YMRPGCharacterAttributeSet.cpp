#include "YMRPGCharacterAttributeSet.h"

UYMRPGCharacterAttributeSet::UYMRPGCharacterAttributeSet()
£ºHealth(100.f)
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



void UYMRPGCharacterAttributeSet::GetLifeTimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifeTimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UYMRPGCharacterAttributeSet, Damage, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME(UYMRPGCharacterAttributeSet, Level, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME(UYMRPGCharacterAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME(UYMRPGCharacterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME(UYMRPGCharacterAttributeSet, Mana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME(UYMRPGCharacterAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME(UYMRPGCharacterAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME(UYMRPGCharacterAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME(UYMRPGCharacterAttributeSet, Attack, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME(UYMRPGCharacterAttributeSet, Defense, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME(UYMRPGCharacterAttributeSet, Exp, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME(UYMRPGCharacterAttributeSet, MaxExp, COND_None, REPNOTIFY_Always);
}

bool UYMRPGCharacterAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
    Super::PreGameplayEffectExecute(Data);

    HealBeforeAttributeChange = GetHealth();
    HealBeforeAttributeChange = GetMaxHealth();

    return true;
}

void UYMRPGCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    const FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();

	AActor* EffectInstigator = Context.GetInstigator();
    AActor* EffectCauser = Context.GetEffectCauser();

    if (Data.EvaluatedData.Atttribute == GetDamageAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth() - GetDamage(), 0.f, GetMaxHealth()));
        SetDamage(0.0f);
    }
    else if(Data.EvaluatedData.Atttribute == GetHealthAttribute())
    {
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
    }
    else if(Data.EvaluatedData.Atttribute == GetMaxHealthAttribute())
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
}

void UYMRPGCharacterAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
}

void UYMRPGCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
}

void UYMRPGCharacterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
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
