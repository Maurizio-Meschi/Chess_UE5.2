// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/GFieldElement/ManageField.h"

void AElementsToManageField::ResetTileArray()
{
	for (int32 i = 0; i < TileArray.Num(); i++)
	{
		TileArray[i]->Destroy();
	}
	TileArray.Empty();
}

void AElementsToManageField::ResetBotPieces()
{
	for (int32 i = 0; i < BotPieces.Num(); i++)
	{
		BotPieces[i]->Destroy();
	}
	BotPieces.Empty();
}

void AElementsToManageField::ResetHumanPlayerPieces()
{
	for (int32 i = 0; i < HumanPlayerPieces.Num(); i++)
	{
		HumanPlayerPieces[i]->Destroy();
	}
	HumanPlayerPieces.Empty();
}

void AElementsToManageField::ResetTileMarked()
{
	for (int32 i = 0; i < TileMarked.Num(); i++)
	{
		if (TileMarked[i]->GetTileStatus() == ETileStatus::MARKED)
			TileMarked[i]->SetTileStatus(-1, ETileStatus::EMPTY);

		if (TileMarked[i]->GetTileStatus() == ETileStatus::MARKED_TO_CAPTURE)
			TileMarked[i]->SetTileStatus(1, ETileStatus::OCCUPIED);
	}
	TileMarked.Empty();
}

void AElementsToManageField::TileMarkedDestroy()
{
	for (int32 i = 0; i < TileMarkedSpawn.Num(); i++)
	{
		TileMarkedSpawn[i]->Destroy();
	}
	TileMarkedSpawn.Empty();
}

void AElementsToManageField::ResetCheckArray()
{
	CheckArray.Empty();
}

void AElementsToManageField::ResetCheckArrayTile()
{
	for (int32 i = 0; i < CheckArrayTile.Num(); i++)
	{
		CheckArrayTile[i]->SetStatusCheckmate(-1, EStatusCheckmate::NEUTRAL);
	}
	CheckArrayTile.Empty();
}

void AElementsToManageField::ResetMap()
{
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

void AElementsToManageField::ResetAll()
{
	for (int32 i = 0; i < TileMarkedSpawn.Num(); i++)
	{
		TileMarkedSpawn[i]->Destroy();
	}

	TileMarkedSpawn.Empty();
	TileMarked.Empty();
	CheckArrayTile.Empty();
	CheckArray.Empty();
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
