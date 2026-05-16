#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"


#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayAbilitySpec.h"


#include "YMRPGCharacterBase.generated.h"


/*
* AYMRPGCharacterBase
* 
* The base character pawn class used by this project.
* Responsible for sending events to pawn components.
* New Behavior should be added via pawn components when possible.
*/

class UYMRPGComboComponent;
class UYMRPGAbilitySystemComponent;
class AYMRPGPlayerController;
class AYMRPGPlayerState;
class UYMRPGGameplayAbility;
class UYMRPGCharacterAttributeSet;

UCLASS(Abstract)
class YMRPG_API AYMRPGCharacterBase : public ACharacter, public IAbilitySystemInterface, public IGameplayCueInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	AYMRPGCharacterBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "YMRPG|Character")
	AYMRPGPlayerController* GetPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "YMRPG|Character")
	AYMRPGPlayerState* GetPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "YMRPG|Character")
	FORCEINLINE UYMRPGAbilitySystemComponent* GetYMRPGAbilitySystemComponent() const { return AbilityComponent; }


	UFUNCTION(BlueprintCallable, Category = "YMRPG|Character")
	FORCEINLINE UYMRPGComboComponent* GetYMRPGComboComponent() const { return ComboComponent; }

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;


	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="YMRPG|Ability", meta=(AllowPrivateAccess="true"))
	TMap<FGameplayTag, TSubclassOf<UYMRPGGameplayAbility>> AbilitiesToAdd;

	TMap<FGameplayTag, FGameplayAbilitySpecHandle> AbilitiesToActive;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "YMRPG|GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UYMRPGAbilitySystemComponent> AbilityComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "YMRPG|Character", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UYMRPGComboComponent> ComboComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "YMRPG|GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UYMRPGCharacterAttributeSet> CharacterSet;
};