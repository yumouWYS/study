#include "UI_InventoryPanel.h"

#include "YMRPGCharacterBase.h"
#include "UI_InventorySlot.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "YMRPGInventoryComponent.h"

void UUI_InventoryPanel::NativeConstruct()
{
	Super::NativeConstruct();

	if (AYMRPGCharacterBase* CharacterBase = UI_GetYMRPGCharacterBase())
	{
		CharacterBase->GetYMRPGInventoryComponent()->OnInventoryItemChanged.BindUObject(this, &ThisClass::OnInventoryItemChanged);

		CharacterBase->CallServerDownLoadInfo();

	}

}

void UUI_InventoryPanel::NativeDestruct()
{
	Super::NativeDestruct();
	if (AYMRPGCharacterBase* CharacterBase = UI_GetYMRPGCharacterBase())
	{
		CharacterBase->GetYMRPGInventoryComponent()->OnInventoryItemChanged.Unbind();
	}
}

void UUI_InventoryPanel::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UUI_InventoryPanel::OnInventoryItemChanged(const TArray<FYMRPGInventoryItem>& InInventoryItems)
{
	//第一次更新创建元素
	if(InventoryArray->GetChildrenCount() == 0)
	{
		//30个插槽，5行，每行六个
		for (size_t i = 0; i < 5; i++)
		{
			for (size_t j = 0; j < 6; j++)
			{
				if (UUI_InventorySlot* SlotWidget = CreateWidget<UUI_InventorySlot>(GetWorld(), InventorySlotClass))
				{
					//1到30 分别对应数组的元素
					SlotWidget->InventoryId = i * 6 + j;
					if (UUniformGridSlot* GridSlot = InventoryArray->AddChildToUniformGrid(SlotWidget, i, j))
					{
						
					}
				}
			}
		}
	}

	if(InventoryArray->GetChildrenCount() > 0)
	{
		for (size_t i = 0; i < 5; i++)
		{
			for (size_t j = 0; j < 6; j++)
			{
				if (UUI_InventorySlot* InSlot = Cast<UUI_InventorySlot>(InventoryArray->GetChildAt(i * 6 + j)))
				{
					//如果数组里面有数据就去更新它
					if (InInventoryItems[i * 6 + j].IsValid())
					{
						InSlot->Num->SetText(FText::FromString(FString::FromInt(InInventoryItems[i * 6 + j].ItemCount)));
						InSlot->Num->SetVisibility(ESlateVisibility::Visible);


						InSlot->SlotIcon->SetBrushFromTexture(InInventoryItems[i * 6 + j].RPGItem->ItemIcon);
						InSlot->SlotIcon->SetVisibility(ESlateVisibility::Visible);
					}
					else
					{
						//没有数据就隐藏

						InSlot->Num->SetVisibility(ESlateVisibility::Hidden);
						InSlot->SlotIcon->SetVisibility(ESlateVisibility::Hidden);
						InSlot->SlotIcon->SetBrushResourceObject(NULL);

					}
				}
			}
		}
	}
}
