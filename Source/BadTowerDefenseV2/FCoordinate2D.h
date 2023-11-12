// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FCoordinate2D.generated.h"

USTRUCT(BlueprintType)
struct FCoordinate2D {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Coordinates")
	int32 X;
	UPROPERTY(BlueprintReadWrite, Category = "Coordinates")
	int32 Y;

	// gonna need some special functions here...
	FCoordinate2D& operator+=(const FCoordinate2D& rhs) {
		X += rhs.X;
		Y += rhs.Y;

		return *this;
	}

	friend bool operator==(const FCoordinate2D& lhs, const FCoordinate2D& rhs) {
		return lhs.X == rhs.X && lhs.Y == rhs.Y;
	}

	friend bool operator!=(const FCoordinate2D& lhs, const FCoordinate2D& rhs) {
		return !(lhs == rhs);
	}

	friend bool operator<(const FCoordinate2D& lhs, const FCoordinate2D& rhs) {
		return Tie(lhs.X, lhs.Y) < Tie(rhs.X, rhs.Y);
	}

	friend uint32 GetTypeHash(const FCoordinate2D& obj) {
		return obj.X * 402653189 ^ obj.Y * 805306457;
	}
};
