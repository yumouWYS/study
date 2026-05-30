#pragma once


#include "GameplayEffectExecutionCalculation.h"
#include "YMRPGDamageExecution_Defense.generated.h"

class UObject;

UCLASS()
class YMRPG_API UYMRPGDamageExecution_Defense : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UYMRPGDamageExecution_Defense();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};