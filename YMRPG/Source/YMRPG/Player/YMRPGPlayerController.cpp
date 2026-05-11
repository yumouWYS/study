// Copyright Epic Games, Inc. All Rights Reserved.


#include "YMRPGPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "YMRPG.h"
#include "Widgets/Input/SVirtualJoystick.h"


#include "YMRPGAbilitySystemComponent.h"
#include "YMRPGPlayerState.h"
#include "YMRPGHUD.h"
#include "YMRPGCharacterBase.h"



AYMRPGPlayerController::AYMRPGPlayerController(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{

}

void AYMRPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogYMRPG, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void AYMRPGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

UYMRPGAbilitySystemComponent* AYMRPGPlayerController::GetYMRPGAbilitySystemComponent() const
{
	AYMRPGCharacterBase* YMRPGCharacter = Cast<AYMRPGCharacterBase>(GetPawn());	
	if (YMRPGCharacter)
	{
		return YMRPGCharacter->GetYMRPGAbilitySystemComponent();
	}
	return nullptr;
}

AYMRPGPlayerState* AYMRPGPlayerController::GetYMRPGPlayerState() const
{
	return CastChecked<AYMRPGPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

AYMRPGHUD* AYMRPGPlayerController::GetYMRPGHUD() const
{
	return CastChecked<AYMRPGHUD>(GetHUD(), ECastCheckedType::NullAllowed);
}

void AYMRPGPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UYMRPGAbilitySystemComponent* YMRPGASC = GetYMRPGAbilitySystemComponent())
	{
        YMRPGASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}
