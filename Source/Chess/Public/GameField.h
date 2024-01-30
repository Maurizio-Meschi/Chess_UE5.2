// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tile.h"
#include "Pieces/Bishop.h"
#include "Pieces/ChessPawn.h"
#include "Pieces/King.h"
#include "Pieces/Knight.h"
#include "Pieces/Queen.h"
#include "Pieces/Rook.h"
#include "ChessPieces.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameField.generated.h"


UCLASS()
class CHESS_API AGameField : public AActor
{
	GENERATED_BODY()

	static constexpr int32 SECOND_ROW_FIELD = 2;
	static constexpr int32 PENULTIMATE_ROW_FIELD = 6;
	static constexpr int32 LAST_ROW_FIELD = 8;

public:
	UPROPERTY(Transient)
	TArray<ATile*> TileArray;
	
	UPROPERTY(Transient)
	TArray<AChessPieces*> PiecesArray;

	/*
	UPROPERTY(Transient)
	TArray<ARook*> Rook;

	UPROPERTY(Transient)
	TArray<AKing*> King;

	UPROPERTY(Transient)
	TArray<AKnight*> Knight;

	UPROPERTY(Transient)
	TArray<AQueen*> Queen;

	UPROPERTY(Transient)
	TArray<ABishop*> Bishop;

	UPROPERTY(Transient)
	TArray<AChessPawn*> Pawn;
	*/

	// Given a position returns a tile
	UPROPERTY(Transient)
	TMap<FVector2D, ATile*> TileMap;

	UPROPERTY(Transient)
	TMap<FVector2D, AChessPieces*> PiecesMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float NormalizedCellPadding;

	static const int32 NOT_ASSIGNED = -1;

	// size of field
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Size;

	// TSubclassOf template class that provides UClass type safety
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<ATile>> TileClass;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<ARook>> ChessRook;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AKing>> ChessKing;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AKnight>> ChessKnight;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AQueen>> ChessQueen;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<ABishop>> ChessBishop;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AChessPawn>> ChessPawn;

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

	// Generrate the chess piece in x,y coordinates
	void GenerateChessPieceInXYPosition(int32 x, int32 y, TSubclassOf<AChessPieces> TileClass, EPieceColor color);

	// return a (x,y) position given a hit (click) on a field tile
	FVector2D GetPosition(const FHitResult& Hit);

	// return the array of tile pointers
	TArray<ATile*> GetTileArray();

	// return the array of chess piece pointers
	TArray<AChessPieces*>& GetPiecesArray();

	// return a relative position given (x,y) position
	FVector GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const;

	// return (x,y) position given a relative position
	FVector2D GetXYPositionByRelativeLocation(const FVector& Location) const;

	// checking if is a valid field position
	inline bool IsValidPosition(const FVector2D Position) const;
};
