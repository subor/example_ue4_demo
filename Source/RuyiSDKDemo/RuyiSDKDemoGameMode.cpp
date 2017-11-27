// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "RuyiSDKDemoGameMode.h"
#include "RuyiSDKDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARuyiSDKDemoGameMode::ARuyiSDKDemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
