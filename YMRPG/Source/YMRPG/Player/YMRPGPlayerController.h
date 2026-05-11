// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "YMRPGPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(Config= Game, Meta = (ShortToolTip = "The Base Player Controller class for this project"))
class AYMRPGPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    	/** Constructor */
	AYMRPGPlayerController(const FObjectInitializer& ObjectInitializer);
	
protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

public:
	UFUNCTION(BlueprintCallable, Category="YMRPG|PlayerController")
	class UYMRPGAbilitySystemComponent* GetYMRPGAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "YMRPG|PlayerController")
	class AYMRPGPlayerState* GetYMRPGPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "YMRPG|PlayerController")
	class AYMRPGHUD* GetYMRPGHUD() const;

	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

};
