// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FCoordinate2D.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RandomWalkLibrary.generated.h"


/**
 *
 */
UCLASS()
class BADTOWERDEFENSEV2_API URandomWalkLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


	UFUNCTION()
	static bool IsSelfAvoiding(TArray<FCoordinate2D> walk, int32 mapSize);

	UFUNCTION()
	static TArray<FCoordinate2D> ShortWalk(int32 mapSize, const FRandomStream & stream);

	UFUNCTION(BlueprintCallable)
	static TArray<FCoordinate2D> DimerizationWalk(int32 mapSize, const FRandomStream& stream);

	UFUNCTION(BlueprintCallable)
	static TArray<FCoordinate2D> DijkstraRandomPath(const FCoordinate2D& start, const FCoordinate2D& end, const FRandomStream& stream, int32 dimensions = 8);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<FCoordinate2D> GetOutOfBoundsNeighbors(FCoordinate2D tile, int32 dimensions = 8);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<FCoordinate2D> GetAllNeighbors(const  FCoordinate2D& tile, int32 dimensions = 8);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FCoordinate2D FindNeighborInNextChunk(const FCoordinate2D& node, const FCoordinate2D& nextChunkCoordinate, const FRandomStream& stream, int32 dimensions = 8);


private:
	UFUNCTION()
	static TArray<FCoordinate2D> GetHexNeighbors(FCoordinate2D start, int32 dimensions = 8);
};
