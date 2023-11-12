// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FCoordinate2D.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MapUtilitiesLibrary.generated.h"

/**
 * 
 */
UCLASS()
class BADTOWERDEFENSEV2_API UMapUtilitiesLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static bool IsNonCornerEdgeTile(int32 X, int32 Y, int32 dimensions);

	UFUNCTION(BlueprintCallable)
	static int32 ConvertCoordinatesToIndex(int32 X, int32 Y, int32 dimensions = 8);

	UFUNCTION(BlueprintCallable)
	static FCoordinate2D ConvertIndexToCoordinates(int32 index, int32 dimensions = 8);
};
