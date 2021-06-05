// Copyright Epic Games, Inc. All Rights Reserved.

#include "BPP_ProjectGameMode.h"
#include "BPP_ProjectHUD.h"
#include "BPP_ProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABPP_ProjectGameMode::ABPP_ProjectGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ABPP_ProjectHUD::StaticClass();
}
