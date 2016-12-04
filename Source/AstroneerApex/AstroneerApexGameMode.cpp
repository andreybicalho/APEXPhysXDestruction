// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "AstroneerApex.h"
#include "AstroneerApexGameMode.h"
#include "AstroneerApexHUD.h"
#include "AstroneerApexCharacter.h"

AAstroneerApexGameMode::AAstroneerApexGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AAstroneerApexHUD::StaticClass();
}
