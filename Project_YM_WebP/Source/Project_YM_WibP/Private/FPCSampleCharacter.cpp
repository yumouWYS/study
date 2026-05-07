


#include "FPCSampleCharacter.h"


// Sets default values
AFPCSampleCharacter::AFPCSampleCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFPCSampleCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFPCSampleCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFPCSampleCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


