// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/GFieldElement/ManageField.h"

void AElementsToManageField::TileMarkedDestroy()
{
	for (int32 i = 0; i < TileMarkedSpawn.Num(); i++)
	{
		TileMarkedSpawn[i]->Destroy();
	}
	TileMarkedSpawn.Empty();
}

void AElementsToManageField::ResetAll()
{
	for (int32 i = 0; i < TileMarkedSpawn.Num(); i++)
	{
		TileMarkedSpawn[i]->Destroy();
	}

	TileMarkedSpawn.Empty();
	KingArray.Empty();
	HumanPlayerPieces.Empty();
	BotPieces.Empty();
	TileArray.Empty();

	for (auto Element : TileMap)
	{
		ATile* Tile = Element.Value;
		Tile->Destroy();
	}
	TileMap.Empty();

	for (auto Element : PiecesMap)
	{
		AChessPieces* Piece = Element.Value;
		Piece->Destroy();
	}
	PiecesMap.Empty();
}
