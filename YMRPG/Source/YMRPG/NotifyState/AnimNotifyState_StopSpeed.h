#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_StopSpeed.generated.h"

/*
*
*/

UCLASS(meta = (DisplayName = "Stop Speed"))
class YMRPG_API UAnimNotifyState_StopSpeed : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	UAnimNotifyState_StopSpeed();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
    float MinAnalogWalkSpeed;
	float MaxWalkSpeed;
};