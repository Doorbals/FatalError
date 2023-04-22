// Copyright Epic Games, Inc. All Rights Reserved.

#include "FatalErrorGameMode.h"
#include "FatalErrorCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFatalErrorGameMode::AFatalErrorGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
