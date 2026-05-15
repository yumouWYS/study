#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "YMRPGAttributeSet.generated.h"

class AActor;
class UYMRPGAbilitySystemComponent;
class UObject;
class UWorld;
struct FGameplayEffectSpec;


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


DECLARE_MULTICAST_DELEGATE_SixParams(
    FYMRPGAttributeEvent,
    AActor* /*EffectInstigator*/,
    AActor* /*EffectCauser*/,
    const FGameplayEffectSpec* /*EffectSpec*/,
    float /*EffectMagnitude*/,
    float /*OldValue*/,
    float /*NewValue*/
);




UCLASS()
class YMRPG_API UYMRPGAttributeSet : public UAttributeSet
{
    GENERATED_BODY()
public:
    UYMRPGAttributeSet();

    UWorld* GetWorld() const override;

    UYMRPGAbilitySystemComponent* GetYMRPGAbilitySystemComponent() const;
};
