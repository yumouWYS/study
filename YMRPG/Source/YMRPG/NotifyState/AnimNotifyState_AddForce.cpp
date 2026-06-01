#include "AnimNotifyState_AddForce.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UAnimNotifyState_AddForce::UAnimNotifyState_AddForce()
	:Super()
	, DirectionForce(1.0f, 0.0f, 0.0f)
	, ForceSize(180000.0f)
	, TotalDurationConsuming(0.0f)
	, ForceSizeConsuming(0.0f)	
{
}

void UAnimNotifyState_AddForce::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if(ACharacter* Character = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		FVector NowFirection = GetCurrentCharacterDirection(Character);

		Character->GetCharacterMovement()->AddImpulse(NowFirection * ForceSize, true);
	}
}

void UAnimNotifyState_AddForce::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UAnimNotifyState_AddForce::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if(ACharacter* Character = Cast<ACharacter>(MeshComp->GetOuter()))
	{
		Character->GetCharacterMovement()->StopMovementImmediately();
	}
}

FVector UAnimNotifyState_AddForce::GetCurrentCharacterDirection(ACharacter* InCharacter)
{
	FVector V(FVector::ZeroVector);
	if (InCharacter)
	{
		if (DirectionForce.X != 0.f)
		{
			V += InCharacter->GetActorForwardVector() * DirectionForce.X;
		}
		if(DirectionForce.Y != 0.f)
		{
			V += InCharacter->GetActorRightVector() * DirectionForce.Y;
		}
		if(DirectionForce.Z != 0.f)
		{
			V += InCharacter->GetActorUpVector() * DirectionForce.Z;
		}
	}
	return V;
}
