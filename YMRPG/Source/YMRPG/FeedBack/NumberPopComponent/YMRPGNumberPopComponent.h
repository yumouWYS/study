#pragma once	

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "YMRPGNumberPopComponent.generated.h"

class UObject;
struct FFrame;

USTRUCT(BlueprintType)
struct FYMRPGNumberPopRequest
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YMRPG|Number Pops")
    FVector WorldLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YMRPG|Number Pops")
    FGameplayTagContainer SourceTags;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YMRPG|Number Pops")
    FGameplayTagContainer TargetTags;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YMRPG|Number Pops")
    int32 NumberToDisplay = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YMRPG|Number Pops")
    bool bIsCriticalDamage = false;

    FYMRPGNumberPopRequest()
        :WorldLocation(ForceInitToZero)
    {

    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "YMRPG|Number Pops")
    FColor ColorToDisplay = FColor::Red;  
};


UCLASS(abstract)
class UYMRPGNumberPopComponent : public UActorComponent
{
    GENERATED_BODY()  
public:
    UYMRPGNumberPopComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UFUNCTION(BlueprintCallable, Category = "YMRPG|Number Pops")
    virtual void AddNumberPop(const FYMRPGNumberPopRequest& NewRequest) {};
};