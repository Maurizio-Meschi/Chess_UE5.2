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
#include "Containers/Map.h"
#include "GameField.generated.h"

USTRUCT()
struct FGFieldTSubClass
{
	GENERATED_BODY()

public:
	FGFieldTSubClass();									

	FGFieldTSubClass& operator=(const FGFieldTSubClass& Other)
	{
		if (this != &Other)
		{
			// Copia gli array di oggetti TSubclassOf
			TileClass = Other.TileClass;
			TileClassMarked = Other.TileClassMarked;
			TileClassPieceToCapture = Other.TileClassPieceToCapture;
			ChessRook = Other.ChessRook;
			ChessKing = Other.ChessKing;
			ChessKnight = Other.ChessKnight;
			ChessQueen = Other.ChessQueen;
			ChessBishop = Other.ChessBishop;
			ChessPawn = Other.ChessPawn;
		}
		return *this;
	}

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<ATile>> TileClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATile> TileClassMarked;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATile> TileClassPieceToCapture;

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
};

UCLASS()
class CHESS_API AGameField : public AActor
{
	GENERATED_BODY()

	static constexpr int32 SECOND_ROW_FIELD = 2;
	static constexpr int32 PENULTIMATE_ROW_FIELD = 6;
	static constexpr int32 LAST_ROW_FIELD = 8;

private:
	UPROPERTY(Transient)
	TArray<ATile*> TileArray;

	UPROPERTY(Transient)
	TArray<AChessPieces*> BotPieces;

	// Vector with tile marked
	UPROPERTY(Transient)
	TArray<ATile*> TileMarked;

	//TODO: mettere dinamicamente il materiale e togliere questo vettore
	UPROPERTY(Transient)
	TArray<ATile*> TileMarkedSpawn;

	// Given a position returns a tile
	UPROPERTY(Transient)
	TMap<FVector2D, ATile*> TileMap;

	// Given a position returns a piece
	UPROPERTY(Transient)
	TMap<FVector2D, AChessPieces*> PiecesMap;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float NormalizedCellPadding;

	// size of field
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Size;

	// TSubclassOf template class that provides UClass type safety
	UPROPERTY(EditAnywhere)
	FGFieldTSubClass GameFieldSubClass;

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
	TArray<ATile*>& GetTileArray();

	TArray<AChessPieces*>& GetBotPieces() { return BotPieces; }

	TArray<ATile*>& GetTileMarked() { return TileMarked; }

	TArray<ATile*>& GetTileMarkedSpawn() { return TileMarkedSpawn; }

	TMap<FVector2D, ATile*>& GetTileMap() { return TileMap; }

	TMap<FVector2D, AChessPieces*>& GetPiecesMap() { return PiecesMap; }

	void AddBotPieces(AChessPieces* Piece) { BotPieces.Add(Piece); }

	void AddTileMarked(ATile* Tile) { TileMarked.Add(Tile); }

	void AddTileMarkedSpawn(ATile* Tile) { TileMarkedSpawn.Add(Tile); }

	void AddTileMap(FVector2D Position, ATile* Tile) { TileMap.Add(Position, Tile); }

	void AddPiecesMap(FVector2D Position, AChessPieces* Piece) { PiecesMap.Add(Position, Piece); }

	void TileMapRemove(FVector2D Position) { TileMap.Remove(Position); }

	void PiecesMapRemove(FVector2D Position) { PiecesMap.Remove(Position); }

	void BotPiecesRemove(AChessPieces* Piece) { BotPieces.Remove(Piece); BotPieces.Shrink(); }

	// return a relative position given (x,y) position
	FVector GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const;

	// return (x,y) position given a relative position
	FVector2D GetXYPositionByRelativeLocation(const FVector& Location) const;

	// 
	void ResetTileMarked();

	void TileMarkedDestroy();

	// checking if is a valid field position
	inline bool IsValidPosition(const FVector2D Position) const;
};
