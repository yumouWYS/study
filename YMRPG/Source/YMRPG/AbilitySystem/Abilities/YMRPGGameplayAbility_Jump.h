#pragma once

#include "YMRPGGameplayAbility.h"
#include "YMRPGGameplayAbility_Jump.generated.h"

class UObject;
struct FFrame;
struct FGameplayTagContainer;
struct FGameplayAbilityActorInfo;


/*
* UYMRPGGameplayAbility_Jump
*
* A simple gameplay ability that causes the character to jump when activated.
*/

UCLASS(Blueprintable)
class YMRPG_API UYMRPGGameplayAbility_Jump : public UYMRPGGameplayAbility
{
	GENERATED_BODY()

public:
	UYMRPGGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer);


protected:
	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags,
		const FGameplayTagContainer* TargetTags,
		OUT FGameplayTagContainer* OptionalRelevantTags
	) const override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

	UFUNCTION(BlueprintCallable, Category = "YMRPG|Ability|Jump")
	void CharacterJumpStart();

	UFUNCTION(BlueprintCallable, Category = "YMRPG|Ability|Jump")
	void CharacterJumpStop();
};