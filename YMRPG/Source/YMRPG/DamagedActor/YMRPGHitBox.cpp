#include "YMRPGHitBox.h"

#include "Components/BoxComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "YMRPGCharacterBase.h"
#include "Abilities/GameplayAbilityTypes.h"

UYMRPGHitBox::UYMRPGHitBox(const FObjectInitializer& ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = false;

    HitCollisionRootComponent= CreateDefaultSubobject<USceneComponent>(TEXT("HitCollisionRootComponent"));
    RootComponent = HitCollisionRootComponent;

    HitBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBoxComponent"));
    HitBoxComponent->SetupAttachment(HitCollisionRootComponent);

    InitialLifeSpan = 4.f;
    bNetLoadOnClient = false;
    bReplicates = false;
}

void UYMRPGHitBox::HandleDamage(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FhitResult& SweepResult)
{

}

UPrimitiveComponent* UYMRPGHitBox::GetHitDamage() const
{
    return HitBoxComponent;
}

void UYMRPGHitBox::SetHitDamageRelativePosition(const FVector& InNewPosition)
{
    if (HitBoxComponent)
    {
        HitBoxComponent->SetRelativeLocation(InNewPosition);
    }
}

void UYMRPGHitBox::SetBoxExtent(const FVector& InNewBoxExtent)
{
    if (HitBoxComponent)
    {
        HitBoxComponent->SetBoxExtent(InNewBoxExtent);
    }
}

bool UYMRPGHitBox::IsExist(AYMRPGCharacterBase* InCharacter) const
{
    return AttackedCharacters.Contains(InCharacter);
}

void UYMRPGHitBox::BeginPlay()
{
    Super::BeginPlay();

}

void UYMRPGHitBox::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
