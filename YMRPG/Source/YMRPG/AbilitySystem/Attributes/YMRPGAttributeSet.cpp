#include "YMRPGAttributeSet.h"
#include "YMRPGAbilitySystemComponent.h"

UYMRPGAttributeSet::UYMRPGAttributeSet()
{
}

UWorld* UYMRPGAttributeSet::GetWorld() const
{

	const UObject* Outer = GetOuter();
	check(Outer);
	return Outer->GetWorld();
}

UYMRPGAbilitySystemComponent* UYMRPGAttributeSet::GetYMRPGAbilitySystemComponent() const
{
	return Cast<UYMRPGAbilitySystemComponent> (GetOwningAbilitySystemComponent());
}
