#pragma once

#include "CoreMinimal.h"
#include "YMRPGAttributeSet.h"
#include "YMRPGCharacterAttributeSet.generated.h"


/*
* UYMRPGCharacterAttributeSet
* 
* 
*/
UCLASS()
class YMRPG_API UYMRPGCharacterAttributeSet : public UYMRPGAttributeSet
{
	GENERATED_BODY()
public:
	UYMRPGCharacterAttributeSet();

	ATTRIBUTE_ACCESSORS(UYMRPGCharacterAttributeSet, Damage);

    ATTRIBUTE_ACCESSORS(UYMRPGCharacterAttributeSet, Level);

	ATTRIBUTE_ACCESSORS(UYMRPGCharacterAttributeSet, Health);
    ATTRIBUTE_ACCESSORS(UYMRPGCharacterAttributeSet, MaxHealth);

    ATTRIBUTE_ACCESSORS(UYMRPGCharacterAttributeSet, Mana);
    ATTRIBUTE_ACCESSORS(UYMRPGCharacterAttributeSet, MaxMana);

	ATTRIBUTE_ACCESSORS(UYMRPGCharacterAttributeSet, Stamina);
	ATTRIBUTE_ACCESSORS(UYMRPGCharacterAttributeSet, MaxStamina);

    ATTRIBUTE_ACCESSORS(UYMRPGCharacterAttributeSet, Attack);
	ATTRIBUTE_ACCESSORS(UYMRPGCharacterAttributeSet, Defense);

	ATTRIBUTE_ACCESSORS(UYMRPGCharacterAttributeSet, Exp);
	ATTRIBUTE_ACCESSORS(UYMRPGCharacterAttributeSet, MaxExp);


	mutable FYMRPGAttributeEvent OnHealthChanged;

    mutable FYMRPGAttributeEvent OnMaxHealthChanged;

    mutable FYMRPGAttributeEvent OnOutOfHealth;

	mutable FYMRPGAttributeEvent OnManaChanged;

	mutable FYMRPGAttributeEvent OnMaxManaChanged;



	//void GetLifeTimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


protected:
	UFUNCTION()
	virtual void OnRep_Damage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    virtual void OnRep_Level(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Mana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxMana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Stamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Attack(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Defense(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Exp(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxExp(const FGameplayAttributeData& OldValue);


	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:
	UPROPERTY(BlueprintReadOnly, Category = "YMRPG|Attributes", ReplicatedUsing = OnRep_Damage, Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Damage;

	UPROPERTY(BlueprintReadOnly, Category = "YMRPG|Attributes", ReplicatedUsing = OnRep_Level, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Level;

	UPROPERTY(BlueprintReadOnly, Category = "YMRPG|Attributes", ReplicatedUsing = OnRep_Health, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, Category = "YMRPG|Attributes", ReplicatedUsing = OnRep_MaxHealth, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	bool bOutOfHealth;

	float MaxHealthBeforeAttributeChange;
	float HealthBeforeAttributeChange;

	UPROPERTY(BlueprintReadOnly, Category = "YMRPG|Attributes", ReplicatedUsing = OnRep_Mana, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Mana;

	UPROPERTY(BlueprintReadOnly, Category = "YMRPG|Attributes", ReplicatedUsing = OnRep_MaxMana, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxMana;

	UPROPERTY(BlueprintReadOnly, Category = "YMRPG|Attributes", ReplicatedUsing = OnRep_Stamina, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Stamina;

	UPROPERTY(BlueprintReadOnly, Category = "YMRPG|Attributes", ReplicatedUsing = OnRep_MaxStamina, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxStamina;

	UPROPERTY(BlueprintReadOnly, Category = "YMRPG|Attributes", ReplicatedUsing = OnRep_Attack, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Attack;

	UPROPERTY(BlueprintReadOnly, Category = "YMRPG|Attributes", ReplicatedUsing = OnRep_Defense, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Defense;

	UPROPERTY(BlueprintReadOnly, Category = "YMRPG|Attributes", ReplicatedUsing = OnRep_Exp, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Exp;

	UPROPERTY(BlueprintReadOnly, Category = "YMRPG|Attributes", ReplicatedUsing = OnRep_MaxExp, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxExp;
};