// Fill out your copyright notice in the Description page of Project Settings.


#include "RandomWalkLibrary.h"
#include "MapUtilitiesLibrary.h"

struct GraphNode {
	int32 weight = TNumericLimits<int32>::Max();
	int32 distance = TNumericLimits<int32>::Max();
};


bool URandomWalkLibrary::IsSelfAvoiding(TArray<FCoordinate2D> walk, int32 mapSize)
{
	auto data = TSet<FCoordinate2D>(walk);
	return mapSize + 1 == data.Num();
}

TArray<FCoordinate2D> URandomWalkLibrary::ShortWalk(int32 mapSize, const FRandomStream& stream)
{
	static auto deltas = TArray<FCoordinate2D>({ {1,0}, {0,1}, {-1,0}, {0,-1} });

	auto visitedPositions = TArray<FCoordinate2D>();
	auto result = TArray<FCoordinate2D>();
	auto feasibleDeltas = TArray<FCoordinate2D>();

	auto zero = FCoordinate2D(0, 0);
	result.Emplace(zero);
	visitedPositions.Emplace(zero);

	for (int i = 0; i < mapSize; i++) {
		feasibleDeltas.Empty();
		for (auto& delta : deltas) {
			auto potential = FCoordinate2D(delta.X + result.Last().X, delta.Y + result.Last().Y);
			if (!visitedPositions.Contains(potential)) {
				feasibleDeltas.Emplace(potential);
			}
		}

		if (feasibleDeltas.IsEmpty()) {
			break;
		}

		auto next = feasibleDeltas[stream.RandRange(0, feasibleDeltas.Num() - 1)];
		visitedPositions.Emplace(next);
		result.Emplace(next);
	}

	return result;
}

TArray<FCoordinate2D> URandomWalkLibrary::DimerizationWalk(int32 mapSize, const FRandomStream& stream)
{
	if (mapSize <= 3) {
		return ShortWalk(mapSize, stream);
	}

	auto result = TArray<FCoordinate2D>();

	while (!IsSelfAvoiding(result, mapSize)) {
		auto pathOne = DimerizationWalk(mapSize / 2, stream);
		auto pathTwo = DimerizationWalk(mapSize - mapSize / 2, stream);

		for (auto& node : pathTwo) {
			node += pathOne.Last();
		}
		pathTwo.RemoveAt(0);
		pathOne.Append(pathTwo);

		result = pathOne;
	}

	return result;
}

/// <summary>
/// Find a random path through a Hex-based grid using Dijkstra's Pathfinding Algorithm. This is achieved by assigning random weights to all navigable paths and aiming for a minimal length path
/// </summary>
/// <param name="start"></param>
/// <param name="end"></param>
/// <param name="stream"></param>
/// <param name="dimensions"></param>
/// <returns></returns>
TArray<FCoordinate2D> URandomWalkLibrary::DijkstraRandomPath(FCoordinate2D start, FCoordinate2D end, const FRandomStream& stream, int32 dimensions) {
	int32 mapSize = dimensions * dimensions;
	TArray<GraphNode> weightMap;
	// Using a 1D array for performance and simplicity...
	weightMap.Init(GraphNode(), mapSize);

	TArray<bool> visited;
	visited.Init(false, mapSize);

	auto startIndex = UMapUtilitiesLibrary::ConvertCoordinatesToIndex(start.X, start.Y);
	auto endIndex = UMapUtilitiesLibrary::ConvertCoordinatesToIndex(end.X, end.Y);

	weightMap[startIndex] = GraphNode(0, 0);
	weightMap[endIndex] = GraphNode(0, 0);

	// We want to make this high, but not so high that our absolute limit becomes a viable path
	constexpr int32 MAX_WEIGHT = TNumericLimits<int32>::Max() / 2;

	// Apply random weights...
	// TODO: We may want this randomness to be based on some kind of heat map determined by the direction that we are travelling
	for (int i = 1; i < dimensions - 1; i++) {
		for (int j = 1; j < dimensions - 1; j++) {
			auto index = UMapUtilitiesLibrary::ConvertCoordinatesToIndex(i, j);
			weightMap[index].weight = stream.RandRange(1, MAX_WEIGHT);
		}
	}

	TArray<FCoordinate2D> path;

	// When we select neighbors, we have to calculate 6 positions (yay hexes....) include (-1, 1) and (1,-1) in terms of relative coordinates

	TQueue<FCoordinate2D> unvisitedNodes;
	unvisitedNodes.Enqueue(start);
	int neighborIndex = 0;

	auto currentPosition = start;
	while (!unvisitedNodes.IsEmpty()) {
		unvisitedNodes.Peek(currentPosition);
		auto currentIndex = UMapUtilitiesLibrary::ConvertCoordinatesToIndex(currentPosition.X, currentPosition.Y);
		visited[currentIndex] = true;

		auto neighbors = GetHexNeighbors(currentPosition, dimensions);

		for (auto& neighbor : neighbors) {
			neighborIndex = UMapUtilitiesLibrary::ConvertCoordinatesToIndex(neighbor.X, neighbor.Y);
			if (!visited[neighborIndex]) {
				if (weightMap[currentIndex].distance + weightMap[neighborIndex].weight < weightMap[neighborIndex].distance) {
					weightMap[neighborIndex].distance = weightMap[currentIndex].distance + weightMap[neighborIndex].weight;
				}
				unvisitedNodes.Enqueue(neighbor);
			}
		}

		unvisitedNodes.Pop();
	}

	// The weight to distance ratio is now completely calculated
	// We just have to backtrack until we are back at our destination
	currentPosition = end;
	while (currentPosition != start) {
		// get the neighbors
		auto neighbors = GetHexNeighbors(currentPosition, dimensions);

		auto nearestNeighbor = neighbors[0];
		for (auto& neighbor : neighbors) {
			neighborIndex = UMapUtilitiesLibrary::ConvertCoordinatesToIndex(neighbor.X, neighbor.Y);
			auto nearestIndex = UMapUtilitiesLibrary::ConvertCoordinatesToIndex(nearestNeighbor.X, nearestNeighbor.Y);
			// Need currentNearestNeighbors distance and current neighbors nearest distance
			if (weightMap[neighborIndex].distance < weightMap[nearestIndex].distance) {
				nearestNeighbor = neighbor;
			}
		}

		currentPosition = nearestNeighbor;
		path.Add(currentPosition);
	}


	return path;
}

TArray<FCoordinate2D> URandomWalkLibrary::GetOutOfBoundsNeighbors(FCoordinate2D tile, int32 dimensions)
{
	auto neighbors = TArray<FCoordinate2D>();
	if (tile.X == 0) {
		neighbors.Emplace(tile.X - 1, tile.Y);
		if (tile.Y == 0) {
			neighbors.Emplace(tile.X - 1, tile.Y - 1);
		}
	}
	if (tile.X == dimensions - 1) {
		neighbors.Emplace(tile.X + 1, tile.Y);
		if (tile.Y == dimensions - 1) {
			neighbors.Emplace(tile.X + 1, tile.Y + 1);
		}
	}

	if (tile.Y == 0) {
		neighbors.Emplace(tile.X, tile.Y - 1);
	}

	if (tile.Y == dimensions - 1) {
		neighbors.Emplace(tile.X, tile.Y + 1);
	}

	return neighbors;
}

TArray<FCoordinate2D> URandomWalkLibrary::GetHexNeighbors(FCoordinate2D start, int32 dimensions)
{
	auto neighbors = TArray<FCoordinate2D>();

	if (start.X > 0) {
		neighbors.Emplace(start.X - 1, start.Y);
		if (start.Y > 0) {
			neighbors.Emplace(start.X - 1, start.Y - 1);
		}
	}

	if (start.X < dimensions - 1) {
		neighbors.Emplace(start.X + 1, start.Y);
		if (start.Y < dimensions - 1) {
			neighbors.Emplace(start.X + 1, start.Y + 1);
		}
	}

	if (start.Y > 0) {
		neighbors.Emplace(start.X, start.Y - 1);
	}

	if (start.Y < dimensions - 1) {
		neighbors.Emplace(start.X, start.Y + 1);
	}

	return neighbors;
}
