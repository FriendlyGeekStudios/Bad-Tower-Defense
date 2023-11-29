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
	auto index = dimensions * X + Y;
	UE_LOG(LogTemp, Log, TEXT("Converted Coordinate (%d, %d) to index %d"), X, Y, index);
	return index;
}

FCoordinate2D UMapUtilitiesLibrary::ConvertIndexToCoordinates(int32 index, int32 dimensions)
{
	UE_LOG(LogTemp, Display, TEXT("Converting Index %d to Coordinates (%d, %d)"), index, index % dimensions, index / dimensions);
	return FCoordinate2D(index % dimensions, index / dimensions);
}

FCoordinate2D UMapUtilitiesLibrary::ConvertGlobalCoordinateToChunkCoordinate(const FCoordinate2D& location, int32 dimensions)
{
	
	auto x = (location.X / dimensions);
	auto y = (location.Y / dimensions);

	if (location.X < 0 && location.X % dimensions) {
		x -= 1;
	}

	if (location.Y < 0 && location.Y % dimensions) {
		y -= 1;
	}

	auto result = FCoordinate2D(x, y);
	UE_LOG(LogTemp, Display, TEXT("Converting Global Coordinate (%d, %d) to Chunk Coordinate: (%d, %d)"), location.X, location.Y, result.X, result.Y);
	return result;
}

FCoordinate2D UMapUtilitiesLibrary::ConvertGlobalCoordinateToChunkLocalCoordinate(const FCoordinate2D& location, int32 dimensions)
{
	auto result = FCoordinate2D(FMath::Abs(location.X) % dimensions, FMath::Abs(location.Y) % dimensions);
	UE_LOG(LogTemp, Display, TEXT("Convert Global Coordinate (%d, %d) to Chunk Local Coordinate (%d, %d)"), location.X, location.Y, result.X, result.Y);

	return result;
}
