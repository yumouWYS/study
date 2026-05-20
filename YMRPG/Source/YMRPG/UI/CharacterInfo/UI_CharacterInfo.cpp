#include "UI_CharacterInfo.h"

#include "YMRPGAbilitySystemComponent.h"
#include "YMRPGCharacterBase.h"
#include "YMRPGCharacterAttributeSet.h"

#include "Components/TextBlock.h"

void UUI_CharacterInfo::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUI_CharacterInfo::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUI_CharacterInfo::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (AYMRPGCharacterBase* Character = UI_GetYMRPGCharacterBase())
	{
		if (UYMRPGAbilitySystemComponent* ASC = Character->GetYMRPGAbilitySystemComponent())
		{
			if (const UYMRPGCharacterAttributeSet* AttributeSet = ASC->GetSet<UYMRPGCharacterAttributeSet>())
			{
				if (Health)
				{
					Health->SetText(FText::Format(
						FText::FromString(TEXT("{0}/{1}")),
						FText::AsNumber(AttributeSet->GetHealth()),
						FText::AsNumber(AttributeSet->GetMaxHealth())));
				}

				if (Mana)
				{
					Mana->SetText(FText::Format(
						FText::FromString(TEXT("{0}/{1}")),
						FText::AsNumber(AttributeSet->GetMana()),
						FText::AsNumber(AttributeSet->GetMaxMana())));
				}

				if (Stamina)
				{
					Stamina->SetText(FText::Format(
						FText::FromString(TEXT("{0}/{1}")),
						FText::AsNumber(AttributeSet->GetStamina()),
						FText::AsNumber(AttributeSet->GetMaxStamina())));
				}

				if (Attack)
				{
					Attack->SetText(FText::AsNumber(AttributeSet->GetAttack()));
				}

				if (Defense)
				{
					Defense->SetText(FText::AsNumber(AttributeSet->GetDefense()));
				}

				if (Exp)
				{
					Exp->SetText(FText::Format(
						FText::FromString(TEXT("{0}/{1}")),
						FText::AsNumber(AttributeSet->GetExp()),
						FText::AsNumber(AttributeSet->GetMaxExp())));
				}
			}
		}
	}


}
