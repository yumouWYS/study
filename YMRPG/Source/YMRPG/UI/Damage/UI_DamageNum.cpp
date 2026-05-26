#include "UI_DamageNum.h"
#include "Components/TextBlock.h"

void UUI_DamageNum::NativeConstruct()
{
	PlayWidgetAnim(TEXT("FadeAnimation"));
}

void UUI_DamageNum::UpdateNum(const FText& InText)
{
	if (DamageNum)
	{
		DamageNum->SetText(InText);
	}
}


void UUI_DamageNum::UpdateNumColor(const FLinearColor& InColor)
{
	if (DamageNum)
	{
		DamageNum->SetColorAndOpacity(FSlateColor(InColor));
	}
}