#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "YMRPGCharacterBase.generated.h"


/*
* AYMRPGCharacterBase
* 
* The base character pawn class used by this project.
* Responsible for sending events to pawn components.
* New Behavior should be added via pawn components when possible.
*/

UCLASS(abstract)
class YMRPG_API AYMRPGCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AYMRPGCharacterBase(const FObjectInitializer& ObjectInitializer);
};