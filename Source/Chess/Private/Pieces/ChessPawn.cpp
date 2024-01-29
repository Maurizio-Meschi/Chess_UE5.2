// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Pieces/ChessPawn.h"
#include "../../Public/GameField.h"

AChessPawn::AChessPawn()
{
	Value = 1;
	First_play = 1;
}

void AChessPawn::LegalMove()
{
	// get the coordinates of the pawn
	FVector2D ChessPawnXYposition = PieceGridPosition;
	int32 x = ChessPawnXYposition[0];
	int32 y = ChessPawnXYposition[1];
	int32 row = 0;
	int32 XYPositionInArray = (x + row) * 8 + y;

	//TODO: avere l'owner della pedina
	int32 tmp_owner = 0;
	// get the tile in the game field 
	TArray<ATile*> TileArray = GField->GetTileArray();

		// check if the next vertical tile is empty and if true, mark the tile
		row = 1;
		XYPositionInArray = (x + row) * 8 + y;
		if (TileArray[XYPositionInArray]->GetTileStatus() == ETileStatus::EMPTY)
		{
			TileArray[XYPositionInArray]->SetTileStatus(tmp_owner, ETileStatus::MARKED);
		}
		// check if the next second vertical tile is empty and if true, mark the tile
		row = 2;
		XYPositionInArray = (x + row) * 8 + y;
		if (TileArray[XYPositionInArray]->GetTileStatus() == ETileStatus::EMPTY && First_play)
		{
			TileArray[XYPositionInArray]->SetTileStatus(tmp_owner, ETileStatus::MARKED);
		}
}

void AChessPawn::Capture()
{

}