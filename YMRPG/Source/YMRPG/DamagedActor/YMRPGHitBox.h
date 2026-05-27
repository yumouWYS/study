#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "YMRPGHitBox.generated.h"

class YMRPGCharacterBase;


UCLASS()
class UYMRPGHitBox : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category= "HitCollision", meta = (AllowPricateAccess = "ture"))
	class USceneComponent* HitCollisionRootComponent;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "HitCollision", meta = (AllowPricateAccess = "ture"))
	class UBoxComponent* HitCollisionBox;

public:
	UYMRPGHitBox(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	virtual void HandleDamage(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FhitResult& SweepResult
	);

	//返回碰撞盒子的函数
	virtual UPrimitiveComponent* GetHitDamage() const;

	//设置碰撞盒子的位置
    void SetHitDamageRelativePosition(const FVector& InNewPosition);

	//设置碰撞盒子的大小
	void SetBoxExtent(const FVector& InNewBoxExtent);

	//设置GAS标签，告诉系统，我们要激发哪一个对应的能力
	void SetBuffs(const TArray<FGameplayTagContainer>& InBuffs) { Buffers = InBuffs; };

	bool IsExist(AYMRPGCharacterBase* InCharacter) const;

protected:

	virtual void BeginPlay()override;
	virtual void Tick(float DeltaTime)override;

protected:
	UPROPERTY()
	TArray<FGameplayTagContainer> Buffs;

	UPROPERTY()
	TArray<TWeakObjectPtr<AYMRPGCharacterBase>> AttackedCharacters;
};