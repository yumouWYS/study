#include "UI_InventorySlot.h"
#include "UI_ICONDragDrog.h"
#include "UI_EquipmentSlot.h"

#include "Blueprint/DragDropOperation.h"

#include "YMRPGCharacterBase.h"
#include "YMRPGEquipment.h"

#include "Components/Image.h"   
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "YMRPGInventoryComponent.h"
#include "YMRPGEquipmentComponent.h"

UUI_InventorySlot::UUI_InventorySlot(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
    SlotType = ESlotType::SlotType_InventorySlot;
}

void UUI_InventorySlot::HideInventoryNum()
{
	Num->SetVisibility(ESlateVisibility::Hidden);
}

void UUI_InventorySlot::ShowInventoryNum()
{
	Num->SetVisibility(ESlateVisibility::Visible);
}

void UUI_InventorySlot::OnClickedWidget()
{
    if(AYMRPGCharacterBase* CharacterBase = UI_GetYMRPGCharacterBase())
    {
		CharacterBase->ActiveSkillByInventoryId(InventoryId);
	}
}

/*
FReply UUI_InventorySlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    return FReply();
}

void UUI_InventorySlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
}

bool UUI_InventorySlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    return false;
}
*/