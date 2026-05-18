#pragma once

#include "CoreMinimal.h"
#include "YMRPGComponentBase.h"
#include "NativeGameplayTags.h"
#include "YMRPGHealthComponent.generated.h"

class UYMRPGHealthComponent;
class UYMRPGAbilitySystemComponent;
class UYMRPGCharacterAttributeSet;

class UObject;
struct FFrame;
struct FGameplayEffectSpec;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FYMRPGHealth_DeathEvent, AActor*, OwingActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FYMRPGHealth_AttributeChanged,
	UYMRPGHealthComponent*, HealthComponent,
	float, OldValue,
	float, NewValue,
	AActor*, Instigator
);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_Damage);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageImmunity);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageSelfDestruct);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageFellOutOfWorld);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_YMRPG_Damage_Message);


UENUM(BlueprintType)
enum class EYMRPGDeathState : uint8 
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};


UCLASS(Blueprintable, BlueprintType, ClassGroup = (YMRPG), meta = (BlueprintSpawnableComponent))
class YMRPG_API UYMRPGHealthComponent : public UYMRPGComponentBase
{
	GENERATED_BODY()

public:
	UYMRPGHealthComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category = "YMRPG|Health")
	static UYMRPGHealthComponent* GetHealthComponent(const AActor* Actor) 
	{
		return (Actor ? Actor->FindComponentByClass<UYMRPGHealthComponent>() : nullptr);
	}

	UFUNCTION(BlueprintCallable, Category = "YMRPG|Health")
	void InitializeWithAbilitySystem(UYMRPGAbilitySystemComponent* AbilitySystemComponent);

	UFUNCTION(BlueprintCallable, Category = "YMRPG|Health")
	void UninitializeFromAbilitySystem();

	UFUNCTION(BlueprintPure, Category = "YMRPG|Health")
	float GetHealth() const;

	UFUNCTION(BlueprintPure, Category = "YMRPG|Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintPure, Category = "YMRPG|Health")
	float GetMana() const;

	UFUNCTION(BlueprintPure, Category = "YMRPG|Health")
	float GetMaxMana() const;

	UFUNCTION(BlueprintPure, Category = "YMRPG|Health")
	float GetHealthPercent() const;
	UFUNCTION(BlueprintPure, Category = "YMRPG|Health")
	float GetManaPercent() const;

	UFUNCTION(BlueprintPure, Category = "YMRPG|Health")
	EYMRPGDeathState GetDeathState()const { return DeathState; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "YMRPG|Health", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const { return DeathState != EYMRPGDeathState::NotDead; }

	virtual void StartDeath();

	virtual void FinishDeath();

	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);

public:
	UPROPERTY(BlueprintAssignable, Category = "YMRPG|Health")
	FYMRPGHealth_DeathEvent OnDeathStarted;

	UPROPERTY(BlueprintAssignable, Category = "YMRPG|Health")
	FYMRPGHealth_DeathEvent OnDeathFinished;

	UPROPERTY(BlueprintAssignable, Category = "YMRPG|Health")
	FYMRPGHealth_AttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "YMRPG|Health")
	FYMRPGHealth_AttributeChanged OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "YMRPG|Health")
	FYMRPGHealth_AttributeChanged OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "YMRPG|Health")
	FYMRPGHealth_AttributeChanged OnMaxManaChanged;

protected:
	virtual void OnUnregister() override;

	void ClearGameplayTags();

	UFUNCTION()
	virtual void OnRep_DeathState(EYMRPGDeathState OldValue);

	virtual void HandleHealthChanged(
		AActor* Instigator,
		AActor* Causer,
		const FGameplayEffectSpec* Spec,
		float DamageMagnitude,
		float OldValue,
		float NewValue
	);

	virtual void HandleMaxHealthChanged(
		AActor* Instigator,
		AActor* Causer,
		const FGameplayEffectSpec* Spec,
		float DamageMagnitude,
		float OldValue,
		float NewValue
	);

	virtual void HandleOutOfHealth(
		AActor* Instigator,
		AActor* Causer,
		const FGameplayEffectSpec* Spec,
		float DamageMagnitude,
		float OldValue,
		float NewValue
	);

	virtual void HandleManaChanged(
		AActor* Instigator,
		AActor* Causer,
		const FGameplayEffectSpec* Spec,
		float DamageMagnitude, 
		float OldValue,
		float NewValue
	);

	virtual void HandleMaxManaChanged(
		AActor* Instigator,
		AActor* Causer,
		const FGameplayEffectSpec* Spec, 
		float DamageMagnitude, 
		float OldValue, 
		float NewValue
	);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	EYMRPGDeathState DeathState;

	UPROPERTY()
	TObjectPtr<UYMRPGAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<const UYMRPGCharacterAttributeSet> HealthSet;
};