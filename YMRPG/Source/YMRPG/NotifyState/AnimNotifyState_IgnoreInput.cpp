#include "AnimNotifyState_IgnoreInput.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"

UAnimNotifyState_IgnoreInput::UAnimNotifyState_IgnoreInput()
{
}

void UAnimNotifyState_IgnoreInput::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if(ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		if (Character->GetWorld()->IsNetMode(ENetMode::NM_Client))
		{
			if(Character->GetLocalRole() == ROLE_AutonomousProxy)
			{
				Character->DisableInput(Character->GetWorld()->GetFirstPlayerController());
			}
		}
		else if (Character->GetWorld()->IsNetMode(ENetMode::NM_Standalone) || Character->GetWorld()->IsNetMode(ENetMode::NM_ListenServer))
		{
			Character->DisableInput(Character->GetWorld()->GetFirstPlayerController());
		}
	}
}

void UAnimNotifyState_IgnoreInput::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UAnimNotifyState_IgnoreInput::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

    if(ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		if (Character->GetWorld()->IsNetMode(ENetMode::NM_Client))
		{
			if(Character->GetLocalRole() == ROLE_AutonomousProxy)
			{
				Character->EnableInput(Character->GetWorld()->GetFirstPlayerController());
			}
		}
		else if (Character->GetWorld()->IsNetMode(ENetMode::NM_Standalone) || Character->GetWorld()->IsNetMode(ENetMode::NM_ListenServer))
		{
			Character->EnableInput(Character->GetWorld()->GetFirstPlayerController());
		}
	}
}
