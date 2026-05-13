#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ResetCombo.generated.h"


/*
* 
*/

UCLASS()
class YMRPG_API UAnimNotify_ResetCombo : public UAnimNotify
{
	GENERATED_BODY()
public:

	UAnimNotify_ResetCombo();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};