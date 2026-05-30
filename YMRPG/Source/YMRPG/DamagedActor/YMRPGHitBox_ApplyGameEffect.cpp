#include "YMRPGHitBox_ApplyGameEffect.h"

#include "Components/BoxComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "YMRPGCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AYMRPGHitBox_ApplyGameEffect::AYMRPGHitBox_ApplyGameEffect(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	HitCollisionRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HitCollisionRootComponent"));
	RootComponent = HitCollisionRootComponent;

	HitCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBoxComponent"));
	HitCollisionBox->SetupAttachment(HitCollisionRootComponent);

    InitialLifeSpan = 4.f;
	bNetLoadOnClient = true;
	bReplicates = true;
	SetReplicatingMovement(true);

	ProjectileMovement= CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->SetUpdatedComponent(RootComponent);
    ProjectileMovement->MaxSpeed = 2000.f;
	ProjectileMovement->InitialSpeed = 1600.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
    ProjectileMovement->SetIsReplicated(true);


	HitCollisionType = EYMRPGHitCollisionType::HITCOLLISIONTYPE_SHORT_RANGE_ATTACK;

}

void AYMRPGHitBox_ApplyGameEffect::PreInitCollision(AActor* InMyInstigator)
{
	if (!InMyInstigator)
	{
		return;
	}
	FVector ShootDirection = InMyInstigator->GetActorRotation().Vector();


	switch (HitCollisionType)
	{
        case EYMRPGHitCollisionType::HITCOLLISIONTYPE_SHORT_RANGE_ATTACK:
			ProjectileMovement->MaxSpeed = 0.f;
            ProjectileMovement->InitialSpeed = 0.f;
			ProjectileMovement->ProjectileGravityScale = 0.f;
			break;
		case EYMRPGHitCollisionType::HITCOLLISIONTYPE_DIRECT_LINE:
			RootComponent->SetWorldRotation(FRotator::ZeroRotator);
			ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
			break;
		case EYMRPGHitCollisionType::HITCOLLISIONTYPE_LINE:
			RootComponent->SetWorldRotation(ShootDirection.Rotation());
			ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
			break;
		case EYMRPGHitCollisionType::HITCOLLISIONTYPE_TRACK_LINE:
			ProjectileMovement->bIsHomingProjectile = true;
			ProjectileMovement->HomingAccelerationMagnitude = ProjectileMovement->MaxSpeed;
			ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
			break;
		case EYMRPGHitCollisionType::HITCOLLISIONTYPE_RANGE_LINE:
			ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
			break;
		case EYMRPGHitCollisionType::HITCOLLISIONTYPE_RANGE:
			ProjectileMovement->MaxSpeed = 0.f;
			ProjectileMovement->InitialSpeed = 0.f;
			ProjectileMovement->ProjectileGravityScale = 0.f;
			break;
		case EYMRPGHitCollisionType::HITCOLLISIONTYPE_CHAIN:
			ProjectileMovement->MaxSpeed = 0.f;
			ProjectileMovement->InitialSpeed = 0.f;
			ProjectileMovement->ProjectileGravityScale = 0.f;
			break;
		default:
			break;
	}
}

void AYMRPGHitBox_ApplyGameEffect::HandleDamage(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (OtherActor == GetInstigator())
	{
		return;
	}

	if (AYMRPGCharacterBase* InTarger = Cast<AYMRPGCharacterBase>(OtherActor))
	{
		if (!InTarger->IsNetMode(ENetMode::NM_Client))
		{
			if (IsExist(InTarger))
			{
				return;
			}

			if (UAbilitySystemComponent* InASC = InTarger->GetAbilitySystemComponent())
			{
				FGameplayEffectContextHandle EffectContext = InASC->MakeEffectContext();
				EffectContext.AddInstigator(GetInstigator(), this);
				EffectContext.AddSourceObject(InTarger);

				FGameplayEffectSpecHandle EffectSpecHandle = InASC->MakeOutgoingSpec(EffectClass, 1, EffectContext);

				if (EffectSpecHandle.IsValid())
				{
					InASC->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(), InASC);
				}

				AttackedTargets.AddUnique(InTarger);
			}
		}
	}
}

UPrimitiveComponent* AYMRPGHitBox_ApplyGameEffect::GetHitDamage() const
{
	return HitCollisionBox;
}

void AYMRPGHitBox_ApplyGameEffect::SetHitDamageRelativePosition(const FVector& InNewPosition)
{
	if (HitCollisionBox)
	{
		HitCollisionBox->SetRelativeLocation(InNewPosition);
	}
}

void AYMRPGHitBox_ApplyGameEffect::SetBoxExtent(const FVector& InNewBoxExtent)
{
	if (HitCollisionBox)
	{
		HitCollisionBox->SetBoxExtent(InNewBoxExtent);
	}
}

bool AYMRPGHitBox_ApplyGameEffect::IsExist(AYMRPGCharacterBase* InCharacter) const
{
	return AttackedTargets.Contains(InCharacter);
}

void AYMRPGHitBox_ApplyGameEffect::BeginPlay()
{
	Super::BeginPlay();

	if (UPrimitiveComponent* HitComponent = GetHitDamage())
	{
		HitComponent->SetHiddenInGame(true);
		HitComponent->OnComponentBeginOverlap.AddDynamic(this, &AYMRPGHitBox_ApplyGameEffect::HandleDamage);
	}

}

void AYMRPGHitBox_ApplyGameEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
