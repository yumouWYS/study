#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"

#include "UI_SlotBase.generated.h"

class UImage;
class UButton;

UCLASS()
class YMRPG_API UUI_SlotBase : public UUI_Base
{
	GENERATED_BODY()
public:
	UUI_SlotBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(meta = (BindWidget))
	UImage* SlotIcon;

	UPROPERTY(meta = (BindWidget))
	UButton* ClickButton;

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<class UUI_InformationDescribe> UI_TipClass;

public:

	virtual void NativeConstruct() override;

	void HideSlotIcon();
	void ShowSlotIcon();

	UFUNCTION(BlueprintCallable, BlueprintPure, category = Tip)
	UWidget* getSlotTip();

	UFUNCTION()
	virtual void OnClickedWidget() {};

public:
	FORCEINLINE ESlotType GetSlotType() const { return SlotType; }

protected:
	ESlotType SlotType;
};