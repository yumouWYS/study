// Copyright Epic Games, Inc. All Rights Reserved.

#include "YMRPGGameMode.h"

#include "YMRPGGameState.h"

#include "YMRPGCharacter.h"
#include "YMRPGPlayerController.h"
#include "YMRPGPlayerState.h"

#include "YMRPGHUD.h"

#include "UObject/ConstructorHelpers.h"

AYMRPGGameMode::AYMRPGGameMode()
{
	// stub
	DefaultPawnClass = AYMRPGCharacter::StaticClass();
    PlayerStateClass = AYMRPGPlayerState::StaticClass();
    HUDClass = AYMRPGHUD::StaticClass();
    PlayerControllerClass = AYMRPGPlayerController::StaticClass();
    GameStateClass = AYMRPGGameState::StaticClass();
}
