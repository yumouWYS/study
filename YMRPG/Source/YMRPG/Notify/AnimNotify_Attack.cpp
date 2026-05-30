#include "AnimNotify_Attack.h"
#include "YMRPGHitBox.h"

UAnimNotify_Attack::UAnimNotify_Attack()
{
	HitObjectClass = AYMRPGHitBox::StaticClass();
    BoxExtent = FVector(32.f);
	LifeTime = 4.f;
	InSocketName = TEXT("OpenFire");
}

FString UAnimNotify_Attack::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_Attack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AActor* InCharacter = Cast<AActor>(MeshComp->GetOwner()))
	{
		FVector ComponentLocation = MeshComp->GetComponentLocation();
        FRotator ComponentRotation = MeshComp->GetComponentRotation();

		if (!InCharacter->GetWorld()->IsNetMode(ENetMode::NM_Client))
		{
			FTransform Transform((ComponentRotation + RotationOffset).Quaternion(), ComponentLocation);
			if (AYMRPGHitBox* HitCollision = InCharacter->GetWorld()->SpawnActorDeferred<AYMRPGHitBox>(
				HitObjectClass,
				Transform,
				nullptr,
				Cast<APawn>(InCharacter),
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			))
			{
				//延迟生成，还没生成
				
				//设置激活目标
				HitCollision->SetBoxExtent(BoxExtent);
                HitCollision->SetBuffs(BuffTags);

				//设置相对位置
				FVector RelativeLocation = HitCollision->GetHitDamage()->GetRelativeLocation();
				HitCollision->SetHitDamageRelativePosition(RelativeLocation + RelativeOffsetLocation);

				//设置声明周期
				HitCollision->SetLifeSpan(LifeTime);

				//结束延迟生成
				HitCollision->FinishSpawning(Transform);

				//必须保证对象生成后再附加，否则附加失败
				if (bBind)
				{
                    HitCollision->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, InSocketName);
				}
			}
		}
	}

}
