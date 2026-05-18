#include "YMRPGComboComponent.h"

UYMRPGComboComponent::UYMRPGComboComponent(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

int32 UYMRPGComboComponent::GetComboIndex() const
{
	return ComboIndex;
}

UAnimMontage* UYMRPGComboComponent::GetComboAnimMontage()
{
	LastPlayedMontage = ComboMontages[ComboIndex];
	return ComboMontages[ComboIndex];
}

UAnimMontage* UYMRPGComboComponent::GetLastComboAnimMontage() const
{
	return LastPlayedMontage;
}

void UYMRPGComboComponent::UpdateComboIndex()
{
	ComboIndex++;
    if (ComboIndex > ComboMaxIndex)
	{
		ComboIndex = 0;
	}
}

void UYMRPGComboComponent::ResetComboIndex()
{
	ComboIndex = 0;
}

void UYMRPGComboComponent::SetPressed()
{
	bShortPress = true;
}

void UYMRPGComboComponent::ResetPressed()
{
	bShortPress = false;
}

bool UYMRPGComboComponent::IsStillPressed() const
{
	return bShortPress;
}

void UYMRPGComboComponent::BeginPlay()
{
	Super::BeginPlay();

	check(ComboMontages.Num() > 0);

	ComboMaxIndex = ComboMontages.Num() - 1;
}
