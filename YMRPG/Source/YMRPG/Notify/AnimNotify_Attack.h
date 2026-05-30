#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_Attack.generated.h"

class AYMRPGHitBox;

UCLASS(meta = (DisplayName = "Attack"))
class YMRPG_API UAnimNotify_Attack : public UAnimNotify
{
	GENERATED_BODY()
public:
	UAnimNotify_Attack();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
	) override;

	//触发哪个tag
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General configuration")
	TArray<FGameplayTagContainer> BuffTags;

	//生成哪一个盒体
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General configuration")
	TSubclassOf<AYMRPGHitBox> HitObjectClass;

	//碰撞体的相对位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General configuration")
	FVector RelativeOffsetLocation;

	//碰撞体的相对旋转
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General configuration")
	FRotator RotationOffset;

	//存在的声明周期
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General configuration")
	float LifeTime;

	//生成在骨骼体5的哪一个部分上
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General configuration")
	FName InSocketName;

	//将box绑定在角色身开火点上，静态的还是移动的
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General configuration")
	bool bBind;

	//Box的大小
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Box")
	FVector BoxExtent;
};