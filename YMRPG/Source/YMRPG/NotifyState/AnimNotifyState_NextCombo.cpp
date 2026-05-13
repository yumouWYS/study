#include "AnimNotifyState_NextCombo.h"
#include "YMRPGCharacter.h"
#include "YMRPGComboComponent.h"
#include "YMRPGCharacterBase.h"

UAnimNotifyState_NextCombo::UAnimNotifyState_NextCombo()
{
}

void UAnimNotifyState_NextCombo::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (MeshComp->GetOwner() && MeshComp->GetOuter()->GetWorld()&& !MeshComp->GetOuter()->GetWorld()->IsNetMode(ENetMode::NM_DedicatedServer))
    {
		if(AYMRPGCharacterBase* Character = Cast<AYMRPGCharacterBase>(MeshComp->GetOwner()))
        {
            Character->GetYMRPGComboComponent()->ResetPressed();
        }
    }

	
}

void UAnimNotifyState_NextCombo::NotifyTick(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UAnimNotifyState_NextCombo::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp->GetOwner() && MeshComp->GetOuter()->GetWorld() && !MeshComp->GetOuter()->GetWorld()->IsNetMode(ENetMode::NM_DedicatedServer))
	{
        if (AYMRPGCharacter* Character = Cast<AYMRPGCharacter>(MeshComp->GetOwner()))
		{
			if(Character->GetYMRPGComboComponent()->IsStillPressed())
			{
				Character->ComboMelee();
			}
		}
	}
}
