#include "YMRPGDamageExecution_Defense.h"
#include "YMRPGCharacterAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"

struct FDamageDefenseStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseDamageDefenseDef;

	FDamageDefenseStatics()
	{
		BaseDamageDefenseDef = FGameplayEffectAttributeCaptureDefinition(UYMRPGCharacterAttributeSet::GetDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FDamageDefenseStatics& DamageDefenseStatics()
{
	static FDamageDefenseStatics Statics;
	return Statics;
}

UYMRPGDamageExecution_Defense::UYMRPGDamageExecution_Defense()
{
	RelevantAttributesToCapture.Add(FDamageDefenseStatics().BaseDamageDefenseDef);
}

void UYMRPGDamageExecution_Defense::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const UAbilitySystemComponent* EffectASC = ExecutionParams.GetTargetAbilitySystemComponent();

	const UAbilitySystemComponent* OriginalASC = ExecutionParams.GetSourceAbilitySystemComponent();

	float Defense = 0.f;

	if (const UYMRPGCharacterAttributeSet* EffectArr = Cast<UYMRPGCharacterAttributeSet>(EffectASC->GetAttributeSet(UYMRPGCharacterAttributeSet::StaticClass())))
	{
		Defense = EffectArr->GetDefense();
	}

	float Attack = 0.f;

	if (const UYMRPGCharacterAttributeSet* OriginalArr = Cast<UYMRPGCharacterAttributeSet>(OriginalASC->GetAttributeSet(UYMRPGCharacterAttributeSet::StaticClass())))
	{
        Attack = OriginalArr->GetAttack();	
	}
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
    EvaluateParameters.SourceTags = SourceTags;
    EvaluateParameters.TargetTags = TargetTags;

	float BaseDefenseDamage = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageDefenseStatics().BaseDamageDefenseDef, EvaluateParameters, BaseDefenseDamage);

	const float BaseAirDamageDone = FMath::Max(0.f, BaseDefenseDamage - Defense + Attack);

	if (BaseAirDamageDone > 0)
	{
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UYMRPGCharacterAttributeSet::GetDamageAttribute(),EGameplayModOp::Additive, BaseAirDamageDone));
	}

#endif

}
