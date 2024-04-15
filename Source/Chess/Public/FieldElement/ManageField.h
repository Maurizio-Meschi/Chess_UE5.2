// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MainActor/Tile.h"
#include "../MainActor/ChessPieces.h"
#include "../Pieces/King.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Map.h"

class CHESS_API AElementsToManageField
{
public:
	
	TArray<AChessPieces*> GetAIPieces() { return AIPieces; }
	TArray<AChessPieces*> GetPlayer1Pieces() { return Player1Pieces; }
	TArray<ATile*> GetTileMarked() { return TileMarked; }
	TMap<FVector2D, ATile*>& GetTileMap() { return TileMap; }
	TMap<FVector2D, AChessPieces*>& GetPiecesMap() { return PiecesMap; }

	void AddAIPieces(AChessPieces* Piece) { AIPieces.Add(Piece); }
	void AddTileMarked(ATile* Tile) { TileMarked.Add(Tile); }
	void AddTileMap(FVector2D Position, ATile* Tile) { TileMap.Add(Position, Tile); }
	void AddPiecesMap(FVector2D Position, AChessPieces* Piece) { PiecesMap.Add(Position, Piece); }

	void TileMapRemove(FVector2D Position) { TileMap.Remove(Position); }
	void PiecesMapRemove(FVector2D Position) { PiecesMap.Remove(Position); }
	void AIPiecesRemove(AChessPieces* Piece) { AIPieces.Remove(Piece); AIPieces.Shrink(); }
	void Player1PiecesRemove(AChessPieces* Piece) { Player1Pieces.Remove(Piece); Player1Pieces.Shrink(); }

	void TileMarkedDestroy();
	void ResetFieldData();


protected:

	UPROPERTY(Transient)
	TArray<AChessPieces*> AIPieces;

	UPROPERTY(Transient)
	TArray<AChessPieces*> Player1Pieces;

	UPROPERTY(Transient)
	TArray<ATile*> TileMarked;

	// Given a position returns a tile
	UPROPERTY(Transient)
	TMap<FVector2D, ATile*> TileMap;

	// Given a position returns a piece
	UPROPERTY(Transient)
	TMap<FVector2D, AChessPieces*> PiecesMap;
};