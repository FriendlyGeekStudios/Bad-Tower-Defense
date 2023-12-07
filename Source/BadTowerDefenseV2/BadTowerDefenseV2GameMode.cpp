// Copyright Epic Games, Inc. All Rights Reserved.

#include "BadTowerDefenseV2GameMode.h"
#include "BadTowerDefenseV2PlayerController.h"
#include "BadTowerDefenseV2Character.h"
#include "UObject/ConstructorHelpers.h"

ABadTowerDefenseV2GameMode::ABadTowerDefenseV2GameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ABadTowerDefenseV2PlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Character/BP_CameraPawn"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/Blueprints/Character/BP_CameraController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}