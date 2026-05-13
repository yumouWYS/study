#include "YMRPGGameplayAbility.h"
#include "YMRPGCharacterBase.h"
#include "YMRPGPlayerController.h"
#include "YMRPGAbilitySystemComponent.h"

UYMRPGGameplayAbility::UYMRPGGameplayAbility(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

UYMRPGAbilitySystemComponent* UYMRPGGameplayAbility::GetYMRPGAbilitySystemComponentFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<UYMRPGAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr);
}

AYMRPGCharacterBase* UYMRPGGameplayAbility::GetYMRPGCharacterBaseFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<AYMRPGCharacterBase>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

AYMRPGPlayerController* UYMRPGGameplayAbility::GetYMRPGPlayerControllerFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<AYMRPGPlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr);
}

AController* UYMRPGGameplayAbility::GetControllerFromActorInfo() const
{
	if (CurrentActorInfo)
	{
		if (AController* PC = CurrentActorInfo->PlayerController.Get())
		{
			return PC;
		}

		AActor* TestActor = CurrentActorInfo->OwnerActor.Get();
		while (TestActor)
		{
            if (AController* C = Cast<AController>(TestActor))
			{
				return C;
			}
			
			if(APawn* Pawn = Cast<APawn>(TestActor))
			{
				return Pawn->GetController();
			}

			TestActor = TestActor->GetOwner();
		}
	}
	return nullptr;
}
