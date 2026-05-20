#include "UI_Base.h"

#include "YMRPGCharacterBase.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Animation/WidgetAnimation.h"
#include "GameFramework/PlayerController.h"

UUI_Base::UUI_Base(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
}

void UUI_Base::PlayWidgetAnim(const FString& InAnimName)
{
    if (UWidgetAnimation* Animation = GetNameWidgetAnimation(InAnimName))
    {
        PlayAnimation(Animation);
    }
}

UWidgetAnimation* UUI_Base::GetNameWidgetAnimation(const FString& InAnimName)
{
    if (UWidgetBlueprintGeneratedClass* WidgetClass = Cast<UWidgetBlueprintGeneratedClass>(GetClass()))
    {
        TArray<UWidgetAnimation*> Animations = WidgetClass->Animations;

        UWidgetAnimation** Animation = Animations.FindByPredicate(
            [&](const UWidgetAnimation* OurAnimation)
            {
                return OurAnimation->GetFName().ToString() == (InAnimName + FString("_INST"));
            }
        );
        return Animation ? *Animation : nullptr;
    }
    return nullptr;
}

AYMRPGCharacterBase* UUI_Base::UI_GetYMRPGCharacterBase() const
{
    if (GetWorld())
    {
        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
        if (PlayerController)
        {
            return (PlayerController->GetPawn<AYMRPGCharacterBase>());
        }
    }
    return nullptr;
}
