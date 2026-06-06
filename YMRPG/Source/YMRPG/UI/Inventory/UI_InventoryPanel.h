#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"

#include "Components/UniformGridPanel.h"
#include "YMRPGInventoryComponent.h"

#include "UI_InventoryPanel.generated.h"

class UUI_InventorySlot;

UCLASS()
class YMRPG_API UUI_InventoryPanel : public UUI_Base
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* InventoryArray;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<UUI_InventorySlot> InventorySlotClass;

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void OnInventoryItemChanged(const TArray<FYMRPGInventoryItem>& InInventoryItems);
};