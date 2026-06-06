#include "UI_SlotBase.h"
#include "UI_InformationDescribe.h"
#include "Components/Image.h"
#include "Components/Button.h"

UUI_SlotBase::UUI_SlotBase(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer), SlotType(ESlotType::SlotType_None)
{

}

void UUI_SlotBase::NativeConstruct()
{
	Super::NativeConstruct();
	
	ClickButton->OnPressed.AddDynamic(this, &ThisClass::OnClickedWidget);
}

void UUI_SlotBase::HideSlotIcon()
{
	SlotIcon->SetVisibility(ESlateVisibility::Hidden);
}

void UUI_SlotBase::ShowSlotIcon()
{
	SlotIcon->SetVisibility(ESlateVisibility::Visible);
}

UWidget* UUI_SlotBase::getSlotTip()
{
	if (UI_TipClass)
	{
		UUI_InformationDescribe* TipWidget = CreateWidget<UUI_InformationDescribe>(GetWorld(), UI_TipClass);
		if (TipWidget)
		{
			return TipWidget;
		}
	}
	return nullptr;
}
