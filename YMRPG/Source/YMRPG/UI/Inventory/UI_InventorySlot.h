#pragma once

#include "CoreMinimal.h"

#include "UI_SlotBase.h"

#include "Input/Reply.h"

#include "UI_InventorySlot.generated.h"

class UTextBlock;

UCLASS()
class YMRPG_API UUI_InventorySlot : public UUI_SlotBase
{
	GENERATED_BODY()

	UPROPERTY(editDefaultsOnly, Category = UI)
	TSubclassOf<class UUI_ICONDragDrog> ICONDragDrogClass;

public:

	UUI_InventorySlot(const FObjectInitializer& ObjectInitializer);

	UPROPERTY()
	int32 InventoryId;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Num;

public:
	void HideInventoryNum();

	void ShowInventoryNum();

	virtual void OnClickedWidget()  override;



public:
	/*
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	*/
};