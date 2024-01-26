// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tile.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameField.generated.h"

UCLASS()
class CHESS_API AGameField : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TArray<ATile*> TileArray;

	// Given a position returns a tile
	UPROPERTY(Transient)
	TMap<FVector2D, ATile*> TileMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float NormalizedCellPadding;

	static const int32 NOT_ASSIGNED = -1;

	// size of field
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Size;

	/* size of winning line
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WinSize;
	*/

	// TSubclassOf template class that provides UClass type safety
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATile> TileClass1;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATile> TileClass2;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<ATile>> ChessPieces;

	// tile padding dimension
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CellPadding;

	// tile size
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TileSize;

	// Sets default values for this actor's properties
	AGameField();

	// Called when an instance of this class is placed (in editor) or spawned
	virtual void OnConstruction(const FTransform& Transform) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// generate an empty game field
	void GenerateField();

	// Generrate the tile in x,y coordinates
	void GenerateTileInXYPosition(int32 x, int32 y, TSubclassOf<ATile> TileClass);

	// return a (x,y) position given a hit (click) on a field tile
	FVector2D GetPosition(const FHitResult& Hit);

	// return the array of tile pointers
	TArray<ATile*>& GetTileArray();

	// return a relative position given (x,y) position
	FVector GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const;

	// return (x,y) position given a relative position
	FVector2D GetXYPositionByRelativeLocation(const FVector& Location) const;

	// checking if is a valid field position
	inline bool IsValidPosition(const FVector2D Position) const;
};
