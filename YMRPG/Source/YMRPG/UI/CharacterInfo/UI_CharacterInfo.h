#pragma once

#include "CoreMinimal.h"
#include "UI_Base.h"
#include "UI_CharacterInfo.generated.h"

class UTextBlock;

UCLASS()
class YMRPG_API UUI_CharacterInfo : public UUI_Base
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Health;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Mana;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Stamina;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Attack;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Defense;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Exp;

protected:

    virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

};