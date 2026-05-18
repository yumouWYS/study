#pragma once


#include "CoreMinimal.h"
#include "YMRPGComponentBase.h"
#include "YMRPGComboComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class YMRPG_API UYMRPGComboComponent : public UYMRPGComponentBase
{
	GENERATED_BODY()

public:
    UYMRPGComboComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "YMRPG|Fight|Combo") 
	int32 GetComboIndex() const;

    UFUNCTION(BlueprintCallable, Category = "YMRPG|Fight|Combo")
	UAnimMontage* GetComboAnimMontage();

	UFUNCTION(BlueprintCallable, Category = "YMRPG|Fight|Combo")
	UAnimMontage* GetLastComboAnimMontage() const;

	UFUNCTION(BlueprintCallable, Category = "YMRPG|Fight|Combo")
	void UpdateComboIndex();

	UFUNCTION(BlueprintCallable, Category = "YMRPG|Fight|Combo")
	void ResetComboIndex();

	UFUNCTION(BlueprintCallable, Category = "YMRPG|Fight|Combo")
	void SetPressed();

	UFUNCTION(BlueprintCallable, Category = "YMRPG|Fight|Combo")
	void ResetPressed();

	UFUNCTION(BlueprintCallable, Category = "YMRPG|Fight|Combo")
	bool IsStillPressed() const;


protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "YMRPG|Fight|Combo")
	TArray<UAnimMontage*> ComboMontages;

	UPROPERTY()
	UAnimMontage* LastPlayedMontage = nullptr;

	int32 ComboIndex = 0;

	int32 ComboMaxIndex = 0;

	bool bShortPress = false;


};