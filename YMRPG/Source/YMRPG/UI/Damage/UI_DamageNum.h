#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "UI_DamageNum.generated.h"

class UTextBlock;

UCLASS()
class YMRPG_API UUI_DamageNum : public UUI_Base
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageNum;

protected:

	virtual void NativeConstruct() override;

public:
	void UpdateNum(const FText& InText);
	void UpdateNumColor(const FLinearColor& InColor);

};