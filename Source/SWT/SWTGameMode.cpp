// Copyright Epic Games, Inc. All Rights Reserved.

#include "SWTGameMode.h"
#include "SWTCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASWTGameMode::ASWTGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
