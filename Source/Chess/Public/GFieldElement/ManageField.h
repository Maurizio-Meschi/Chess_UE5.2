// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MainActor/Tile.h"
#include "../MainActor/ChessPieces.h"
#include "../Pieces/King.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Map.h"
#include "ManageField.generated.h"

UCLASS(ABSTRACT)
class CHESS_API AElementsToManageField : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(Transient)
	TArray<ATile*> TileArray;

	UPROPERTY(Transient)
	TArray<AChessPieces*> BotPieces;

	UPROPERTY(Transient)
	TArray<AChessPieces*> HumanPlayerPieces;

	//TODO: mettere dinamicamente il materiale e togliere questo vettore
	UPROPERTY(Transient)
	TArray<ATile*> TileMarkedSpawn;

	UPROPERTY(Transient)
	TArray<AKing*> KingArray;
public:
	// Given a position returns a tile
	UPROPERTY(Transient)
	TMap<FVector2D, ATile*> TileMap;

	// Given a position returns a piece
	UPROPERTY(Transient)
	TMap<FVector2D, AChessPieces*> PiecesMap;
//public:
	// return the array of tile pointers
	TArray<ATile*> GetTileArray() { return TileArray; };
	TArray<AChessPieces*> GetBotPieces() { return BotPieces; }
	TArray<AChessPieces*> GetHumanPlayerPieces() { return HumanPlayerPieces; }
	TArray<ATile*> GetTileMarkedSpawn() { return TileMarkedSpawn; }
	TArray<AKing*> GetKingArray() { return KingArray; }
	TMap<FVector2D, ATile*>& GetTileMap() { return TileMap; }
	TMap<FVector2D, AChessPieces*>& GetPiecesMap() { return PiecesMap; }

	void SetTileMap(TMap<FVector2D, ATile*> Map) { TileMap = Map; }
	void SetPiecesMap(TMap<FVector2D, AChessPieces*> Map) { PiecesMap = Map; }

	void AddBotPieces(AChessPieces* Piece) { BotPieces.Add(Piece); }
	void AddTileArray(ATile* Tile) { TileArray.Add(Tile); }
	void AddTileMarkedSpawn(ATile* Tile) { TileMarkedSpawn.Add(Tile); }
	void AddTileMap(FVector2D Position, ATile* Tile) { TileMap.Add(Position, Tile); }
	void AddPiecesMap(FVector2D Position, AChessPieces* Piece) { PiecesMap.Add(Position, Piece); }

	void TileMapRemove(FVector2D Position) { TileMap.Remove(Position); }
	void PiecesMapRemove(FVector2D Position) { PiecesMap.Remove(Position); }
	void BotPiecesRemove(AChessPieces* Piece) { BotPieces.Remove(Piece); BotPieces.Shrink(); }
	void HumanPlayerPiecesRemove(AChessPieces* Piece) { HumanPlayerPieces.Remove(Piece); HumanPlayerPieces.Shrink(); }

	void TileMarkedDestroy();
	void ResetAll();
};