#include "YMRPGGameplayAbility_Jump.h"
#include "YMRPGCharacterBase.h"

UYMRPGGameplayAbility_Jump::UYMRPGGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UYMRPGGameplayAbility_Jump::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags,
	OUT FGameplayTagContainer* OptionalRelevantTags
) const
{
	if(!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const AYMRPGCharacterBase* Character = Cast<AYMRPGCharacterBase>(ActorInfo->AvatarActor.Get());

	if(!Character && !Character->CanJump())
	{
		return false;
	}

	if(!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UYMRPGGameplayAbility_Jump::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	CharacterJumpStop();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}

void UYMRPGGameplayAbility_Jump::CharacterJumpStart()
{
	if(AYMRPGCharacterBase* Character = GetYMRPGCharacterBaseFromActorInfo())
	{
		if (Character->IsLocallyControlled() && !Character->bPressedJump)
		{
			Character->UnCrouch();
			Character->Jump();
		}
	}
}

void UYMRPGGameplayAbility_Jump::CharacterJumpStop()
{

	if (AYMRPGCharacterBase* Character = GetYMRPGCharacterBaseFromActorInfo())
	{
		if (Character->IsLocallyControlled())
		{
			Character->StopJumping();
		}
	}
}