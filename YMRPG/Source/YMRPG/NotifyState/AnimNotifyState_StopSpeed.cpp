#include "AnimNotifyState_StopSpeed.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UAnimNotifyState_StopSpeed::UAnimNotifyState_StopSpeed()
{
}

void UAnimNotifyState_StopSpeed::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		MaxWalkSpeed = Character->GetCharacterMovement()->MaxWalkSpeed;
		MinAnalogWalkSpeed = Character->GetCharacterMovement()->MinAnalogWalkSpeed;
        Character->GetCharacterMovement()->MaxWalkSpeed = 0;
        Character->GetCharacterMovement()->MinAnalogWalkSpeed = 0;
	}
}

void UAnimNotifyState_StopSpeed::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UAnimNotifyState_StopSpeed::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
        Character->GetCharacterMovement()->MinAnalogWalkSpeed = MinAnalogWalkSpeed;
	}
}