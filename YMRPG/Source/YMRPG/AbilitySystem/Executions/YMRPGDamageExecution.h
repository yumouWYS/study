#pragma once


#include "GameplayEffectExecutionCalculation.h"
#include "YMRPGDamageExecution.generated.h"



class UObject;

UCLASS()
class YMRPG_API UYMRPGDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UYMRPGDamageExecution();

protected:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};