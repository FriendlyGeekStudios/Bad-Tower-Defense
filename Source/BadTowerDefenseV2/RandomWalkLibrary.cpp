// Fill out your copyright notice in the Description page of Project Settings.


#include "RandomWalkLibrary.h"
#include "MapUtilitiesLibrary.h"

// We want to make this high, but not so high that our absolute limit becomes a viable path
constexpr int32 BORDER_INFINITY = TNumericLimits<int32>::Max() / 2;
constexpr int32 MAX_WEIGHT = BORDER_INFINITY / 10;

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
TArray<FCoordinate2D> URandomWalkLibrary::DijkstraRandomPath(const FCoordinate2D& start, const FCoordinate2D& end, const FRandomStream& stream, int32 dimensions) {
	/*TArray<FCoordinate2D> temp;
	temp.Add(start);
	temp.Add(end);
	return temp;*/

	UE_LOG(LogTemp, Log, TEXT("Finding Path from (%d, %d) to (%d, %d)"), start.X, start.Y, end.X, end.Y);

	TArray<FCoordinate2D> frontier;
	frontier.Add(start);
	TMap<FCoordinate2D, FCoordinate2D> cameFrom;
	cameFrom.Add(start, start);
	TMap<FCoordinate2D, int32> costSoFar;
	costSoFar.Add(start, 0);

	if (frontier.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("Frontier is Empty for some reason!"));
		return {};
	}

	FCoordinate2D current;
	while (!frontier.IsEmpty() && frontier[0] != end) {
		current = frontier[0];
		frontier.RemoveAt(0);
		auto neighbors = GetAllNeighbors(current);
		for (auto& neighbor : neighbors) {
			// check that neighbor is valid...
			if (neighbor.X < 0 || neighbor.Y < 0 || neighbor.X >= dimensions || neighbor.Y >= dimensions) {
				// UE_LOG(LogTemp, Log, TEXT("Invalid Neighbor Checked ({%d},{%d})..."), neighbor.X, neighbor.Y); // Should only really encounter this a few times per chunk
				continue;
			}

			auto newCost = costSoFar[current] + stream.RandRange(0, MAX_WEIGHT);
			if (neighbor.X == 0 || neighbor.X == dimensions - 1 || neighbor.Y == 0 || neighbor.Y == dimensions - 1) {
				newCost = BORDER_INFINITY;
			}

			if (neighbor == end) {
				newCost = 0;
			}

			if (!costSoFar.Contains(neighbor) || newCost < costSoFar[neighbor]) {
				costSoFar.FindOrAdd(neighbor);
				costSoFar[neighbor] = newCost;
				cameFrom.FindOrAdd(neighbor);
				cameFrom[neighbor] = current;
				frontier.Add(neighbor);
			}
		}
	}

	// Algorithm is complete... lets backtrack...
	TArray<FCoordinate2D> results;
	current = end;
	while (current != start) {
		if (results.Num() > 64) {
			UE_LOG(LogTemp, Log, TEXT("Failed to find a path for some reason. Would enchroach infinity"));
			break;
		}
		results.Add(current);
		current = cameFrom[current];
	}

	results.Add(current);

	UE_LOG(LogTemp, Log, TEXT("Path for Chunk"));
	for (auto& tile : results) {
		UE_LOG(LogTemp, Log, TEXT("\t(%d, %d)"), tile.X, tile.Y);
	}

	return results;
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

TArray<FCoordinate2D> GetOddRNeighbors(const FCoordinate2D& tile) {
	auto isEven = tile.X % 2 == 0;

	if (isEven) { // even rows...
		return TArray<FCoordinate2D>({
			{tile.X + 1, tile.Y},
			{tile.X, tile.Y - 1},
			{tile.X - 1, tile.Y - 1},
			{tile.X - 1, tile.Y},
			{tile.X - 1, tile.Y + 1},
			{tile.X, tile.Y + 1},
			});

	}

	return TArray<FCoordinate2D>({
			{tile.X + 1, tile.Y},
			{tile.X + 1, tile.Y - 1},
			{tile.X, tile.Y - 1 },
			{tile.X - 1, tile.Y},
			{tile.X, tile.Y + 1},
			{tile.X + 1, tile.Y + 1}
		});
}
TArray<FCoordinate2D> GetEvenRNeighbors(const FCoordinate2D& tile) {
	auto isEven = tile.X % 2 == 0;

	if (!isEven) {
		return TArray<FCoordinate2D>({
			{tile.X + 1, tile.Y},
			{tile.X + 1, tile.Y - 1},
			{tile.X, tile.Y - 1},
			{tile.X - 1, tile.Y},
			{tile.X, tile.Y + 1},
			{tile.X + 1, tile.Y + 1},
			});

	}

	return TArray<FCoordinate2D>({
			{tile.X + 1, tile.Y},
			{tile.X, tile.Y - 1},
			{tile.X - 1, tile.Y - 1 },
			{tile.X - 1, tile.Y},
			{tile.X - 1, tile.Y + 1},
			{tile.X, tile.Y + 1}
		});
}
TArray<FCoordinate2D> GetOddQNeighbors(const FCoordinate2D& tile) {
	auto isEven = tile.X % 2 == 0;

	if (!isEven) {
		return TArray<FCoordinate2D>({
			{tile.X + 1, tile.Y},
			{tile.X + 1, tile.Y - 1},
			{tile.X, tile.Y - 1},
			{tile.X - 1, tile.Y - 1},
			{tile.X - 1, tile.Y},
			{tile.X, tile.Y + 1},
			});

	}

	return TArray<FCoordinate2D>({
			{tile.X + 1, tile.Y + 1},
			{tile.X + 1, tile.Y},
			{tile.X, tile.Y - 1 },
			{tile.X - 1, tile.Y},
			{tile.X - 1, tile.Y + 1},
			{tile.X, tile.Y + 1}
		});
}
TArray<FCoordinate2D> GetEvenQNeighbors(const FCoordinate2D& tile) {
	auto isEven = tile.X % 2 == 0;

	if (!isEven) {
		return TArray<FCoordinate2D>({
			{tile.X + 1, tile.Y + 1},
			{tile.X + 1, tile.Y},
			{tile.X, tile.Y - 1},
			{tile.X - 1, tile.Y},
			{tile.X - 1, tile.Y + 1},
			{tile.X, tile.Y + 1},
			});

	}

	return TArray<FCoordinate2D>({
			{tile.X + 1, tile.Y},
			{tile.X + 1, tile.Y - 1},
			{tile.X, tile.Y - 1 },
			{tile.X - 1, tile.Y - 1},
			{tile.X - 1, tile.Y},
			{tile.X, tile.Y + 1}
		});
}


TArray<FCoordinate2D> URandomWalkLibrary::GetAllNeighbors(const FCoordinate2D& tile, int32 dimensions)
{
	auto parity = tile.Y % 2 == 0; // if even, this will be 0

	if (parity) {
		return TArray<FCoordinate2D>({
			{tile.X - 1, tile.Y},
			{tile.X - 1, tile.Y - 1},
			{tile.X, tile.Y - 1},
			{tile.X + 1, tile.Y},
			{tile.X, tile.Y + 1},
			{tile.X - 1, tile.Y + 1},
			});

	}

	return TArray<FCoordinate2D>({
			{tile.X - 1, tile.Y},
			{tile.X, tile.Y - 1},
			{tile.X + 1, tile.Y - 1 },
			{tile.X + 1, tile.Y},
			{tile.X + 1, tile.Y + 1},
			{tile.X, tile.Y + 1}
		});
}

/// <summary>
/// Finds a neighboring tile that is in another chunk
/// </summary>
/// <param name="node">Global position of the tile we're currently analyzing</param>
/// <param name="nextChunkCoordinate"></param>
/// <param name="stream"></param>
/// <param name="dimensions"></param>
/// <returns></returns>
FCoordinate2D URandomWalkLibrary::FindNeighborInNextChunk(const FCoordinate2D& node, const FCoordinate2D& nextChunkCoordinate, const FRandomStream& stream, int32 dimensions)
{
	UE_LOG(LogTemp, Log, TEXT("Looking for Neighbors for node (%d, %d) in Chunk (%d, %d)"), node.X, node.Y, nextChunkCoordinate.X, nextChunkCoordinate.Y);

	auto neighbors = GetAllNeighbors(node).FilterByPredicate([&](const FCoordinate2D& coord) {
		// Need to make sure we aren't going to grab a tile corner.
		// Honestly. corners are unlikely to be an issue any more with the self-avoiding walk...

		return nextChunkCoordinate == UMapUtilitiesLibrary::ConvertGlobalCoordinateToChunkCoordinate(coord);
		});

	for (auto& neighbor : neighbors) {
		UE_LOG(LogTemp, Log, TEXT("\tPotential Neighbor (%d, %d)"), neighbor.X, neighbor.Y);
	}

	// get random neighbor..
	UE_LOG(LogTemp, Log, TEXT("Found %d potential neighbors"), neighbors.Num());

	if (neighbors.IsEmpty()) {
		return {};
	}

	auto result = neighbors[stream.RandRange(0, neighbors.Num() - 1)];

	UE_LOG(LogTemp, Log, TEXT("\t SELECTED (%d, %d)"), result.X, result.Y);

	return result;

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
