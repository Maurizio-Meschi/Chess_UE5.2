// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/FieldElement/ManageField.h"

void AElementsToManageField::TileMarkedDestroy()
{
	for (int32 i = 0; i < TileMarked.Num(); i++)
	{
		TileMarked[i]->Destroy();
	}
	TileMarked.Empty();
}

void AElementsToManageField::ResetFieldData()
{
	for (int32 i = 0; i < TileMarked.Num(); i++)
	{
		TileMarked[i]->Destroy();
	}

	TileMarked.Empty();
	KingArray.Empty();
	Player1Pieces.Empty();
	AIPieces.Empty();

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
