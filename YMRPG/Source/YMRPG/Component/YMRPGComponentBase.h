#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/GameFrameworkComponent.h"
#include "YMRPGComponentBase.generated.h"

UCLASS(ClassGroup= (Custom), meta = (BlueprintSpawnableComponent))
class YMRPG_API UYMRPGComponentBase : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:

	UYMRPGComponentBase(const FObjectInitializer& ObjectInitializer);
};