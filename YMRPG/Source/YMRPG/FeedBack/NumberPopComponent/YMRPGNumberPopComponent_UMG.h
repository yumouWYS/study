#pragma once	

#include "Components/ControllerComponent.h"
#include "GameplayTagContainer.h"
#include "YMRPGNumberPopComponent.h"

#include "YMRPGNumberPopComponent_UMG.generated.h"

class AYMRPGNumberPopActor;

UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class UYMRPGNumberPopComponent_UMG : public UYMRPGNumberPopComponent
{
	GENERATED_BODY()

public:
	UYMRPGNumberPopComponent_UMG(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void AddNumberPop(const FYMRPGNumberPopRequest& NewRequest) {} override;

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultOnly, Category = "YMRPG|NumberPop", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AYMRPGNumberPopActor> NumberPopWidgetClass;


};