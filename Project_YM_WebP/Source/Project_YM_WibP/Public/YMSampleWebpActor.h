

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "YMSampleWebpActor.generated.h"

UCLASS()
class PROJECT_YM_WIBP_API AYMSampleWebpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AYMSampleWebpActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
