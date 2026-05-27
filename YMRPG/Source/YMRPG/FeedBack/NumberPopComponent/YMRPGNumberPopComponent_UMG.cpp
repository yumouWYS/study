#include "YMRPGNumberPopComponent_UMG.h"
#include "YMRPGNumberPopActor.h"

UYMRPGNumberPopComponent_UMG::UYMRPGNumberPopComponent_UMG(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{

}

void UYMRPGNumberPopComponent_UMG::AddNumberPop(const FYMRPGNumberPopRequest& NewRequest)
{
	if (APlayerController* PC = GetOwner<APlayerController>())
	{
		if (!PC->IsLocalController())
		{
			return;
		}
	}

	FTransform CameraTansform;
	FVector NumberLocation(NewRequest.WorldLocation);

	if (APlayerController* PC = GetOwner<APlayerController>())
	{
		if (APlayerCameraManager* PCM = PC->PlayerCameraManager)
		{
            CameraTansform = FTransform(PCM->GetCameraRotation(), PCM->GetCameraLocation());

			FVector LocationOffset(ForceInitToZero);

			const float RandomMagnitude = 5.f;
			LocationOffset += FMath::RandPointInBox(FBox(-FVector(RandomMagnitude), FVector(RandomMagnitude)));

			NumberLocation += LocationOffset;
		}
	}

	if (UWorld* LocalWorld = GetWorld())
	{
		AYMRPGNumberPopActor* PopActor = LocalWorld->SpawnActor<AYMRPGNumberPopActor>(NumberPopActorClass, NewRequest.WorldLocation, FRotator::ZeroRotator);

		PopActor->UpdateNum(FText::FromString(FString::FromInt(NewRequest.NumberToDisplay)));
        PopActor->UpdateNumColor(NewRequest.ColorToDisplay);
	}

}