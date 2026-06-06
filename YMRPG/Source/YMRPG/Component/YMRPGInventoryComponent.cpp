#include "YMRPGInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "YMRPGPotion.h"
#include "YMRPGEquipment.h"

#include "AbilitySystemComponent.h"
#include "YMRPGGameplayAbility.h"
#include "YMRPGCharacterBase.h"

#include "YMRPGEquipmentComponent.h"

FYMRPGInventoryItem::FYMRPGInventoryItem() : RPGItem(nullptr), ItemCount(0)	
{	
}

bool FYMRPGInventoryItem::IsValid() const
{
	if(RPGItem && ItemCount > 0)
	{
		return true;
	}
	return false;
}


void FYMRPGInventoryItem::ResetSelf()
{
	RPGItem = nullptr;
	ItemCount = 0;
}

UYMRPGInventoryComponent::UYMRPGInventoryComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;

	InventoryItems.AddDefaulted(30);
}

void UYMRPGInventoryComponent::OnRep_InventoryItems()
{
	OnInventoryItemChanged.ExecuteIfBound(InventoryItems);
}

void UYMRPGInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UYMRPGInventoryComponent::ActiveSkillByInventoryId(int32 InventoryId)
{
	if (InventoryItems[InventoryId].IsValid())
	{
		if(UYMRPGPotion* Potion = Cast<UYMRPGPotion>(InventoryItems[InventoryId].RPGItem))
		{
			UAbilitySystemComponent* ASC = Cast<AYMRPGCharacterBase>(GetOwner())->GetAbilitySystemComponent();

			UYMRPGGameplayAbility* GA = Cast<UYMRPGGameplayAbility>(Potion->GrantedAbility->GetDefaultObject());

			FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(FGameplayAbilitySpec(GA));

			ASC->SetRemoveAbilityOnEnd(Handle);

			ASC->TryActivateAbility(Handle);
		}
		else if (UYMRPGEquipment* Equipment = Cast<UYMRPGEquipment>(InventoryItems[InventoryId].RPGItem))
		{
			
		}
	}
}

void UYMRPGInventoryComponent::CallServerDownLoadInfo()
{
	InventoryItemChanged(InventoryItems);
}

void UYMRPGInventoryComponent::InventoryItemChanged_Implementation(const TArray<FYMRPGInventoryItem>& InInventoryItems)
{
	OnInventoryItemChanged.ExecuteIfBound(InInventoryItems);
}



bool UYMRPGInventoryComponent::AddInventoryItem(FYMRPGInventoryItem NewItem)
{
	if(HasAuthority())
	{
		if(!NewItem.IsValid())
		{
			return false;
		}

		//��һ�� ԭ�ȵĲֿ���û��
		bool bFind = false;
		for (size_t i = 0; i < InventoryItems.Num(); i++)
		{
			if (InventoryItems[i].RPGItem == NewItem.RPGItem)
			{
				if (NewItem.RPGItem->MaxCount > 1)
				{

					InventoryItems[i].ItemCount += NewItem.ItemCount;
					bFind = true;
					return true;
				}
			}

		}

		//ȥ��һ�������пյ�λ��
		for (size_t i = 0; i < InventoryItems.Num(); i++)
		{
			if (!InventoryItems[i].IsValid())
			{
				InventoryItems[i] = NewItem;
				return true;
			}
		}
	}
	return false;
}

bool UYMRPGInventoryComponent::HasThisInventoryItem(FYMRPGInventoryItem NewItem)
{
	for (size_t i = 0; i < InventoryItems.Num(); i++)
	{
		if (InventoryItems[i].RPGItem == NewItem.RPGItem
			&& InventoryItems[i].ItemCount == NewItem.ItemCount)
		{
			return true;
		}
	}
	return false;
}

bool UYMRPGInventoryComponent::ReplaceInventoryItem(FYMRPGInventoryItem NewItem, int32 InReplace_ID)
{
	if (HasAuthority())
	{
		InventoryItems[InReplace_ID] = NewItem;
		return true;
	}
	return false;
}

void UYMRPGInventoryComponent::RemoveInventoryItem(int32 Index_Remove)
{
	if (HasAuthority())
	{
		InventoryItems[Index_Remove].ResetSelf();
	}
}

void UYMRPGInventoryComponent::RemoveInventoryItemByItem(FYMRPGInventoryItem NewItem)
{
	if (HasAuthority())
	{
		for (size_t i = 0; i < InventoryItems.Num(); i++)
		{
			if (InventoryItems[i].RPGItem == NewItem.RPGItem)
			{
				//��ȥĿ������
				InventoryItems[i].ItemCount -= NewItem.ItemCount;

				if (InventoryItems[i].ItemCount < 0)
				{
					InventoryItems[i].ResetSelf();
				}
				break;
			}

		}
	}
}

void UYMRPGInventoryComponent::SwapInventoryItem(int32 Index_i, int32 Index_j)
{
	if (HasAuthority())
	{
		InventoryItems.Swap(Index_i, Index_j);
	}
}

UYMRPGItem* UYMRPGInventoryComponent::GetInventoryItemByID(int32 i)
{
	if (InventoryItems.IsValidIndex(i))
	{
		return InventoryItems[i].RPGItem;
	}
	return nullptr;
}

FYMRPGInventoryItem UYMRPGInventoryComponent::GetInventoryItemInfoByID(int32 i)
{
	return InventoryItems[i];
}

void UYMRPGInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UYMRPGInventoryComponent, InventoryItems);
}
