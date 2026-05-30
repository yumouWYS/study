#include "YMRPGHitBox.h"

#include "Components/BoxComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "YMRPGCharacterBase.h"
#include "Abilities/GameplayAbilityTypes.h"

AYMRPGHitBox::AYMRPGHitBox(const FObjectInitializer& ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = false;

    HitCollisionRootComponent= CreateDefaultSubobject<USceneComponent>(TEXT("HitCollisionRootComponent"));
    RootComponent = HitCollisionRootComponent;

    HitCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBoxComponent"));
    HitCollisionBox->SetupAttachment(HitCollisionRootComponent);

    InitialLifeSpan = 4.f;
    bNetLoadOnClient = false;
    bReplicates = false;
}

void AYMRPGHitBox::HandleDamage(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComponent,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    if (GetInstigator() == OtherActor)
    {
        return;
    }
    if (AYMRPGCharacterBase* InPawn = Cast<AYMRPGCharacterBase>(GetInstigator()))
    {
        if (AYMRPGCharacterBase* InTarger = Cast<AYMRPGCharacterBase>(OtherActor))
        {
            if (!InPawn->IsNetMode(ENetMode::NM_Client))
            {
                //传给GAS的事件数据
                FGameplayEventData EventData;
                EventData.Instigator = GetInstigator();
                EventData.Target = InTarger;

                if (IsExist(InTarger))
                {
                    return;
                }

                if (!Buffs.IsEmpty())
                {
                    for (auto& Tmp : Buffs)
                    {
                        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), Tmp.Last(), EventData);
                    }
                }

                AttackedCharacters.AddUnique(InTarger);
            }
        }
    }
}

UPrimitiveComponent* AYMRPGHitBox::GetHitDamage() const
{
    return HitCollisionBox;
}

void AYMRPGHitBox::SetHitDamageRelativePosition(const FVector& InNewPosition)
{
    if (HitCollisionBox)
    {
        HitCollisionBox->SetRelativeLocation(InNewPosition);
    }
}

void AYMRPGHitBox::SetBoxExtent(const FVector& InNewBoxExtent)
{
    if (HitCollisionBox)
    {
        HitCollisionBox->SetBoxExtent(InNewBoxExtent);
    }
}

bool AYMRPGHitBox::IsExist(AYMRPGCharacterBase* InCharacter) const
{
    return AttackedCharacters.Contains(InCharacter);
}

void AYMRPGHitBox::BeginPlay()
{
    Super::BeginPlay();

    if (UPrimitiveComponent* HitComponent = GetHitDamage())
    {
        HitComponent->SetHiddenInGame(true);
        HitComponent->OnComponentBeginOverlap.AddDynamic(this, &AYMRPGHitBox::HandleDamage);
    }

}

void AYMRPGHitBox::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
