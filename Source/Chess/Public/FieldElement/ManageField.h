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
	
	TArray<AChessPieces*> GetPlayer2Pieces() { return Player2Pieces; }
	TArray<AChessPieces*> GetPlayer1Pieces() { return Player1Pieces; }
	TArray<ATile*> GetTileMarked() { return TileMarked; }
	TArray<AKing*> GetKingArray() { return KingArray; }
	TMap<FVector2D, ATile*>& GetTileMap() { return TileMap; }
	TMap<FVector2D, AChessPieces*>& GetPiecesMap() { return PiecesMap; }

	void AddPlayer2Pieces(AChessPieces* Piece) { Player2Pieces.Add(Piece); }
	void AddTileMarked(ATile* Tile) { TileMarked.Add(Tile); }
	void AddTileMap(FVector2D Position, ATile* Tile) { TileMap.Add(Position, Tile); }
	void AddPiecesMap(FVector2D Position, AChessPieces* Piece) { PiecesMap.Add(Position, Piece); }

	void TileMapRemove(FVector2D Position) { TileMap.Remove(Position); }
	void PiecesMapRemove(FVector2D Position) { PiecesMap.Remove(Position); }
	void Player2PiecesRemove(AChessPieces* Piece) { Player2Pieces.Remove(Piece); Player2Pieces.Shrink(); }
	void Player1PiecesRemove(AChessPieces* Piece) { Player1Pieces.Remove(Piece); Player1Pieces.Shrink(); }

	void TileMarkedDestroy();
	void ResetFieldData();


protected:

	UPROPERTY(Transient)
	TArray<AChessPieces*> Player2Pieces;

	UPROPERTY(Transient)
	TArray<AChessPieces*> Player1Pieces;

	//TODO: mettere dinamicamente il materiale e togliere questo vettore
	UPROPERTY(Transient)
	TArray<ATile*> TileMarked;

	UPROPERTY(Transient)
	TArray<AKing*> KingArray;

	// Given a position returns a tile
	UPROPERTY(Transient)
	TMap<FVector2D, ATile*> TileMap;

	// Given a position returns a piece
	UPROPERTY(Transient)
	TMap<FVector2D, AChessPieces*> PiecesMap;
};