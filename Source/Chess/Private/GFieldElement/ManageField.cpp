// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/GFieldElement/ManageField.h"

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
	CheckArray.SetNum(0);
}

void AElementsToManageField::ResetCheckArrayTile()
{
	for (int32 i = 0; i < CheckArrayTile.Num(); i++)
	{
		CheckArrayTile[i]->SEtStatusCheckmate(-1, EStatusCheckmate::NEUTRAL);
	}
	CheckArrayTile.Empty();
}
