#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "YMRPGHitBox_ApplyGameEffect.generated.h"

class AYMRPGCharacterBase;
class UProjectileMovementComponent;

UENUM(BlueprintType)
enum class EYMRPGHitCollisionType :uint8
{
	HITCOLLISIONTYPE_SHORT_RANGE_ATTACK		UMETA(DisplayName="Short range attack"),	//近战攻击

	HITCOLLISIONTYPE_DIRECT_LINE			UMETA(DisplayName="Direct Line"),			//无障碍直线攻击
	HITCOLLISIONTYPE_LINE					UMETA(DisplayName = "Lint"),				//非跟踪类型，类似手枪子弹
	HITCOLLISIONTYPE_TRACK_LINE				UMETA(DisplayName = "Track Line"),			//跟踪类型
	HITCOLLISIONTYPE_RANGE_LINE				UMETA(DisplayName = "Range Line"),			//范围伤害，丢受累
	HITCOLLISIONTYPE_RANGE					UMETA(DisplayName = "Range"),				//范围伤害，类似自爆
	HITCOLLISIONTYPE_CHAIN					UMETA(DisplayName = "Chain"),				//链条类型，持续伤害类型
};

UCLASS()
class YMRPG_API AYMRPGHitBox_ApplyGameEffect : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HitCollision", meta = (AllowPrivateAccess = "ture"))
	class USceneComponent* HitCollisionRootComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HitCollision", meta = (AllowPrivateAccess = "ture"))
	class UBoxComponent* HitCollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HitCollision", meta = (AllowPrivateAccess = "ture"))
	class UProjectileMovementComponent* ProjectileMovement;

public:
	UPROPERTY(EditDefaultsOnly, Category = "HitCollision")
	EYMRPGHitCollisionType HitCollisionType;

	AYMRPGHitBox_ApplyGameEffect(const FObjectInitializer& ObjectInitializer);

	virtual void PreInitCollision(AActor* InMyInstigator);

	UFUNCTION()
	virtual void HandleDamage(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	//返回碰撞盒子的函数
	virtual UPrimitiveComponent* GetHitDamage() const;

	//设置碰撞盒子的位置
	void SetHitDamageRelativePosition(const FVector& InNewPosition);

	//设置碰撞盒子的大小
	void SetBoxExtent(const FVector& InNewBoxExtent);

	//设置Gas标签，告诉系统，我们要激发哪一种对应的能力
	void SetGameplayEffect(TSubclassOf<UGameplayEffect>& InGE) {
		EffectClass = InGE;
	}

	bool IsExist(AYMRPGCharacterBase* InCharacter) const;

protected:
	virtual void BeginPlay()override;
	virtual void Tick(float DeltaTime) override;




protected:
	UPROPERTY()
	TSubclassOf<UGameplayEffect> EffectClass;

	//储存我们的重叠目标
	TArray<TWeakObjectPtr<AYMRPGCharacterBase>> AttackedTargets;

};