#include "YMRPGNumberPopActor.h"	
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "UI_DamageNum.h"

AYMRPGNumberPopActor::AYMRPGNumberPopActor()
{
	DamageNumRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DamageNumRootComponent"));
	RootComponent = DamageNumRootComponent;

	PrimaryActorTick.bCanEverTick = false;

	DamageWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageWidgetComponent"));
    DamageWidgetComponent->SetupAttachment(RootComponent);
	DamageWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InitialLifeSpan = 4.0f;
}


void AYMRPGNumberPopActor::UpdateNum(const FText& Intext)
{
	if (DamageWidgetComponent)
	{
		if (UUI_DamageNum* DamageNumWidget = Cast<UUI_DamageNum>(DamageWidgetComponent->GetUserWidgetObject()))
		{
			DamageNumWidget->UpdateNum(Intext);
		}
	}

}


void AYMRPGNumberPopActor::UpdateNumColor(const FLinearColor& InColor)
{

	if (DamageWidgetComponent)
	{
		if (UUI_DamageNum* DamageNumWidget = Cast<UUI_DamageNum>(DamageWidgetComponent->GetUserWidgetObject()))
		{
			DamageNumWidget->UpdateNumColor(InColor);
		}
	}
}