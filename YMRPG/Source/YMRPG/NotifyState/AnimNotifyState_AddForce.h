#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_AddForce.generated.h"

/*
*
*/


class AYMRPGCharacter;

UCLASS(meta = (DisplayName = "Add Force"))
class YMRPG_API UAnimNotifyState_AddForce : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_AddForce();


	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Add Force Notify State")
	FVector DirectionForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Add Force Notify State")
	float ForceSize;

private:
	//总共消耗的事件，即此通知时长
	float TotalDurationConsuming;
	//每次判定消耗后的残值力，当开始时这个理论上等于ForceSize，结束时等于0
	float ForceSizeConsuming;

	FVector GetCurrentCharacterDirection(ACharacter* InCharacter);

};