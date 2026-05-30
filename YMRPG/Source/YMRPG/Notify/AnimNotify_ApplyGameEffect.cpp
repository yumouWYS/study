#include "AnimNotify_ApplyGameEffect.h"

#include "YMRPGHitBox_ApplyGameEffect.h"
#include "YMRPGCharacterBase.h"

UAnimNotify_ApplyGameEffect::UAnimNotify_ApplyGameEffect():Super()
{
	HitObjectClass = AYMRPGHitBox_ApplyGameEffect::StaticClass();
	BoxExtent = FVector(32.f);
	LifeTime = 4.f;
	InSocketName = TEXT("OpenFire");
}

FString UAnimNotify_ApplyGameEffect::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_ApplyGameEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (AActor* InCharacter = Cast<AActor>(MeshComp->GetOuter()))
	{
		if (!(InCharacter->GetWorld()->GetNetMode() == NM_Client))
		{
			FVector ComponentLocation = MeshComp->GetComponentLocation();
			FRotator ComponentRotation = MeshComp->GetComponentRotation();

			AYMRPGCharacterBase* CheckCharacter = Cast<AYMRPGCharacterBase>(InCharacter);

			//在编辑器下预览时，不会转化为AYMRPGCharacterBase* ，这时会有旋转问题，所以需要修正
			if (!CheckCharacter)
			{
				ComponentRotation += FRotator(0, 90, 0);
			}

			FTransform Transform((ComponentRotation + RotationOffset).Quaternion(), ComponentLocation);

			FString VForward = ComponentLocation.ToString();//可以用于打日志

			if (AYMRPGHitBox_ApplyGameEffect* HitCollision = InCharacter->GetWorld()->SpawnActorDeferred<AYMRPGHitBox_ApplyGameEffect>(
				HitObjectClass,
				Transform,
				nullptr,
				Cast<APawn>(InCharacter),
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			))
			{
				HitCollision->PreInitCollision(InCharacter);

				//激活设置的GE类
                HitCollision->SetGameplayEffect(EffectClass);

                HitCollision->SetBoxExtent(BoxExtent);

				FVector RelativeLocation = HitCollision->GetHitDamage()->GetRelativeLocation();
				HitCollision->SetHitDamageRelativePosition(RelativeLocation + RelativeOffsetLocation);

				HitCollision->SetLifeSpan(LifeTime);

				HitCollision->FinishSpawning(Transform);

				if (bBind)
				{
                    HitCollision->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, InSocketName);
				}
			}
		}
	}
}
