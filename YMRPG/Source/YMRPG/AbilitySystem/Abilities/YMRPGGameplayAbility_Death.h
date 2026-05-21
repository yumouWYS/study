#pragma once

#include "YMRPGGameplayAbility.h"
#include "YMRPGGameplayAbility_Death.generated.h"

class UObject;
struct FFrame;
struct FGameplayAbilityActorInfo;
struct FGameplayEventData;


/*
* UYMRPGGameplayAbility_Death
* 
*	Gameplay ability uesd for handling death
*	Ability is actived automatically via the "GameplayEvent.Death" ability trigger tag
*/
UCLASS(Abstract)
class UYMRPGGameplayAbility_Death : public UYMRPGGameplayAbility
{
	GENERATED_BODY()

public:
	UYMRPGGameplayAbility_Death(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, 
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

	UFUNCTION(BlueprintCallable, Category = "YMRPG|Ability")
	void StartDeath();

	UFUNCTION(BlueprintCallable, Category = "YMRPG|Ability")
	void FinishDeath();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "YMRPG|Death")
	bool bAutoStartDeath;

};
