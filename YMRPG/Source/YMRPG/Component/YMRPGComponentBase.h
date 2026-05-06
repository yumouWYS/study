#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "YMRPGComponentBase.generated.h"

UCLASS(ClassGroup= (Custom), meta = (BlueprintSpawnableComponent))
class YMRPG_API UYMRPGComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:

	UYMRPGComponentBase();
};