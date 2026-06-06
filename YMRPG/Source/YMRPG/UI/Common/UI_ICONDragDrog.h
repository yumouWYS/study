
#pragma once

#include "CoreMinimal.h"

#include "UI_Base.h"
#include "UI_ICONDragDrog.generated.h"

class UImage;
/**
 *
 */
UCLASS()
class YMRPG_API UUI_ICONDragDrog : public UUI_Base
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UImage* ICON;

public:
	void DrawICON(UTexture2D* InICON);


};