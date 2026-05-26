#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"	
#include "YMRPGNumberPopActor.generated.h"

class UWidgetComponent;

UCLASS()
class YMRPG_API AYMRPGNumberPopActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HitCollision", meta = (AllowPrivateAccess= "true"))
	class USceneComponent* DamageNumRootComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess= "true"))
	UWidgetComponent* DamageWidgetComponent;

public:
	AYMRPGNumberPopActor();

	void UpdateNum(const FText& Intext);
	void UpdateNumColor(const FLinearColor& InColor);
};