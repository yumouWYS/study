#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI_Base.generated.h"

class AYMRPGCharacterBase;
class UWidgetAnimation;

UCLASS()
class YMRPG_API UUI_Base : public UUserWidget
{
	GENERATED_BODY()

public:
	UUI_Base(const FObjectInitializer& ObjectInitializer);

protected:
	void PlayWidgetAnim(const FString& InAnimName);

	UWidgetAnimation* GetNameWidgetAnimation(const FString& InAnimName);

	AYMRPGCharacterBase* UI_GetYMRPGCharacterBase() const;
};