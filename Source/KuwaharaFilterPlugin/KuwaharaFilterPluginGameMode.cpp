// Copyright Epic Games, Inc. All Rights Reserved.

#include "KuwaharaFilterPluginGameMode.h"
#include "KuwaharaFilterPluginCharacter.h"
#include "UObject/ConstructorHelpers.h"

AKuwaharaFilterPluginGameMode::AKuwaharaFilterPluginGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
