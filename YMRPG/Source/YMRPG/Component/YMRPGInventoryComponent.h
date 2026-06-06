#pragma once

#include "CoreMinimal.h"
#include "YMRPGItem.h"

#include "Components/GameFrameworkComponent.h"
#include "YMRPGComponentBase.h"

#include "YMRPGInventoryComponent.generated.h"

class AYMRPGCharacterBase;
class UYMRPGAbilitySystemComponent;
class UYMRPGItem;

USTRUCT(BlueprintType)
struct FYMRPGInventoryItem
{
	GENERATED_USTRUCT_BODY()
public:
	FYMRPGInventoryItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RPGInventoryItem)
	UYMRPGItem* RPGItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RPGInventoryItem)
	int32 ItemCount;

	bool IsValid() const;

	void ResetSelf();
};

DECLARE_DELEGATE_OneParam(FOnInventoryItemChangedNative, const TArray<FYMRPGInventoryItem>&);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class YMRPG_API UYMRPGInventoryComponent : public UYMRPGComponentBase
{
	GENERATED_BODY()
public:
	UYMRPGInventoryComponent(const FObjectInitializer& ObjectInitializer);

	//��UI�󶨵Ĵ���
	//�ֿ�Ĵ���
	FOnInventoryItemChangedNative OnInventoryItemChanged;

	UFUNCTION()
	void OnRep_InventoryItems();

protected:

	virtual void BeginPlay() override;


public:

	void ActiveSkillByInventoryId(int32 InventoryId);

	void CallServerDownLoadInfo();

	UFUNCTION(Client, Reliable)
	void InventoryItemChanged(const TArray<FYMRPGInventoryItem>& InInventoryItems);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool AddInventoryItem(FYMRPGInventoryItem NewItem);

	UFUNCTION(BlueprintPure, Category = Inventory)
	bool HasThisInventoryItem(FYMRPGInventoryItem NewItem);

	//���ӵ��ƶ�λ��
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool ReplaceInventoryItem(FYMRPGInventoryItem NewItem, int32 InReplace_ID);
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void RemoveInventoryItem(int32 Index_Remove);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void RemoveInventoryItemByItem(FYMRPGInventoryItem NewItem);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void SwapInventoryItem(int32 Index_i, int32 Index_j);

	//��ȡ�ƶ��ֿ�ָ����λ����Ʒ
	UFUNCTION(BlueprintCallable, Category = Inventory)
	UYMRPGItem* GetInventoryItemByID(int32 i);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	FYMRPGInventoryItem GetInventoryItemInfoByID(int32 i);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_InventoryItems, CateGory = Inventory)
	TArray<FYMRPGInventoryItem> InventoryItems;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;		
};