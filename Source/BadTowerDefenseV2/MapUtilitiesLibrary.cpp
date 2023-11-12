// Fill out your copyright notice in the Description page of Project Settings.


#include "MapUtilitiesLibrary.h"

bool UMapUtilitiesLibrary::IsNonCornerEdgeTile(int32 X, int32 Y, int32 dimensions)
{
	return (Y == 0 || Y == dimensions - 1)
		&& (X != 0 && X != dimensions - 1)
		|| (X == 0 || X == dimensions - 1)
		&& (Y != 0 && Y != dimensions - 1);
}

int32 UMapUtilitiesLibrary::ConvertCoordinatesToIndex(int32 X, int32 Y, int32 dimensions)
{
	return dimensions * X + Y;
}

FCoordinate2D UMapUtilitiesLibrary::ConvertIndexToCoordinates(int32 index, int32 dimensions)
{
	return FCoordinate2D(index % dimensions, index / dimensions);
}
