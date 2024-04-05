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
	
	TArray<AChessPieces*> GetBotPieces() { return BotPieces; }
	TArray<AChessPieces*> GetHumanPlayerPieces() { return HumanPlayerPieces; }
	TArray<ATile*> GetTileMarked() { return TileMarked; }
	TArray<AKing*> GetKingArray() { return KingArray; }
	TMap<FVector2D, ATile*>& GetTileMap() { return TileMap; }
	TMap<FVector2D, AChessPieces*>& GetPiecesMap() { return PiecesMap; }

	void AddBotPieces(AChessPieces* Piece) { BotPieces.Add(Piece); }
	void AddTileMarked(ATile* Tile) { TileMarked.Add(Tile); }
	void AddTileMap(FVector2D Position, ATile* Tile) { TileMap.Add(Position, Tile); }
	void AddPiecesMap(FVector2D Position, AChessPieces* Piece) { PiecesMap.Add(Position, Piece); }

	void TileMapRemove(FVector2D Position) { TileMap.Remove(Position); }
	void PiecesMapRemove(FVector2D Position) { PiecesMap.Remove(Position); }
	void BotPiecesRemove(AChessPieces* Piece) { BotPieces.Remove(Piece); BotPieces.Shrink(); }
	void HumanPlayerPiecesRemove(AChessPieces* Piece) { HumanPlayerPieces.Remove(Piece); HumanPlayerPieces.Shrink(); }

	void TileMarkedDestroy();
	void ResetFieldData();


protected:

	UPROPERTY(Transient)
	TArray<AChessPieces*> BotPieces;

	UPROPERTY(Transient)
	TArray<AChessPieces*> HumanPlayerPieces;

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