// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Pieces/ChessPawn.h"
#include "../../Public/GameField.h"
#include "../Public/Chess_GameMode.h"

AChessPawn::AChessPawn()
{
	Value = 1;
	FirstPlay = true;
}

void AChessPawn::LegalMove(int32 PlayerNumber, bool IsHumanPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("This is Legal Move!"));
	// get the coordinates of the pawn
	FVector2D ChessPawnXYposition = PieceGridPosition;
	int32 x = ChessPawnXYposition[0];
	int32 XMove = 0;
	int32 y = ChessPawnXYposition[1];
	int32 YMove = 0;
	ATile* SelectedTile = nullptr;


	if (GameModeClass != nullptr)
		GMode = Cast<AChess_GameMode>(GWorld->GetAuthGameMode());
	else
		UE_LOG(LogTemp, Error, TEXT("Game Mode is null"));

	// check if the next first vertical tile is empty and if true, mark the tile
	XMove = IsHumanPlayer ? 1 : -1;
	if (CheckCoord(x + XMove, y)) 
	{
		SelectedTile = GMode->GField->TileMap[FVector2D(x + XMove, y)];

		UE_LOG(LogTemp, Error, TEXT("Pezzo da catturare in posizione x+1, y: %d"), SelectedTile->GetTileStatus());

		if (SelectedTile == nullptr)
			UE_LOG(LogTemp, Error, TEXT("No tile found"));

		if (SelectedTile->GetTileStatus() == ETileStatus::EMPTY)
		{
			SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED);
			TileMarked.Add(SelectedTile);
		}
	}

	// check if it is possible to capture an enemy piece
	int32 i = 1;
	for (int32 k = 0; k < 2; k++)
	{
		XMove = IsHumanPlayer ? 1 : -1;
		YMove = IsHumanPlayer ? i : -i;
		if (CheckCoord(x + XMove, y + YMove))
		{
			SelectedTile = GMode->GField->TileMap[FVector2D(x + XMove, y + YMove)];

			if (SelectedTile == nullptr)
				UE_LOG(LogTemp, Error, TEXT("No tile found"));

			UE_LOG(LogTemp, Error, TEXT("Pezzo da catturare in posizione x+1, y+%d: %d"),i, SelectedTile->GetTileStatus());

			if (SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
			{
				AChessPieces* SelectedPiece = GMode->GField->PiecesMap[FVector2D(x + XMove, y + YMove)];
				if (SelectedPiece == nullptr)
					UE_LOG(LogTemp, Error, TEXT("No piece found"));


				if (SelectedPiece->Color == (IsHumanPlayer ? EPieceColor::WHITE : EPieceColor::BLACK))
				{
					SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED_TO_CAPTURE);
					TileMarked.Add(SelectedTile);
				}
			}
		}
		i = -1;
	}
	// check if the next second vertical tile is empty and if true, mark the tile
	if (x == 1)
	{
		XMove = IsHumanPlayer ? 2 : -2;
		if (CheckCoord(x + XMove, y))
		{
			SelectedTile = GMode->GField->TileMap[FVector2D(x + XMove, y)];
			UE_LOG(LogTemp, Error, TEXT("Pezzo da catturare in posizione x+2, y: %d"), SelectedTile->GetTileStatus());

			if (SelectedTile == nullptr)
				UE_LOG(LogTemp, Error, TEXT("No tile found"));

			if (SelectedTile->GetTileStatus() == ETileStatus::EMPTY)
			{
				SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED);
				TileMarked.Add(SelectedTile);
			}
		}
	}
}

void AChessPawn::Capture()
{

}