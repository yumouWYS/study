#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayEffect.h"
#include "AnimNotify_ApplyGameEffect.generated.h"


class AYMRPGHitBox_ApplyGameEffect;

/*
* 
*/


UCLASS(meta = (DisplayName="ApplyGameplayEffect"))
class YMRPG_API UAnimNotify_ApplyGameEffect : public UAnimNotify
{
	GENERATED_BODY()
public:

	UAnimNotify_ApplyGameEffect();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	//应用哪一个GE示例
	UPROPERTY(EditAnywhere, BlueprintReadWrite,CateGory = "General Configuration")
	TSubclassOf<UGameplayEffect> EffectClass;


	//生成哪一个盒体，匹配生成的盒体才能产生相应的效果
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Configuration")
	TSubclassOf<AYMRPGHitBox_ApplyGameEffect> HitObjectClass;

	//碰撞体的相对位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Configuration")
	FVector RelativeOffsetLocation;

	//碰撞体的相对旋转
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Configuration")
	FRotator RotationOffset;

	//生命周期
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