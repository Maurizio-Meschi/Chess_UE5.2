// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Pieces/ChessPawn.h"
#include "../../Public/GameField.h"
#include "../Public/Chess_GameMode.h"

AChessPawn::AChessPawn()
{
	Value = 1;
}

void AChessPawn::LegalMove(int32 PlayerNumber, bool IsHumanPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("This is Legal Move!"));
	// get the coordinates of the pawn
	FVector2D ChessPawnXYposition = PieceGridPosition;
	int32 x = ChessPawnXYposition.X;
	int32 XMove = 0;
	int32 y = ChessPawnXYposition.Y;
	int32 YMove = 0;

	if (GameModeClass != nullptr) 
		GMode = Cast<AChess_GameMode>(GWorld->GetAuthGameMode());
	else
		UE_LOG(LogTemp, Error, TEXT("Game Mode is null"));
	AGameField* Field = GMode->GField;

	// check if the next first vertical tile is empty and if true, mark the tile
	XMove = IsHumanPlayer ? 1 : -1;
	if (CheckCoord(x + XMove, y)) 
	{
		Mark(x + XMove, y, PlayerNumber);
	}

	// check if it is possible to capture an enemy piece
	int32 i = 1;
	for (int32 k = 0; k < 2; k++)
	{
		XMove = IsHumanPlayer ? 1 : -1;
		YMove = IsHumanPlayer ? i : -i;
		if (CheckCoord(x + XMove, y + YMove))
		{
			MarkToCapture(x + XMove, y + YMove, PlayerNumber, IsHumanPlayer);
		}
		i = -1;
	}
	// check if the next second vertical tile is empty and if true, mark the tile
	if (x == 1)
	{
		XMove = IsHumanPlayer ? 2 : -2;
		if (CheckCoord(x + XMove, y))
		{
			Mark(x + XMove, y, PlayerNumber);
		}
	}
}

void AChessPawn::Mark(int32 x, int32 y, int32 PlayerNumber)
{
	ATile* SelectedTile = nullptr;
	AGameField* Field = GMode->GField;

	GMode->CriticalSection.Lock();

	SelectedTile = Field->TileMap[FVector2D(x, y)];

	GMode->CriticalSection.Unlock();

	if (SelectedTile == nullptr)
		UE_LOG(LogTemp, Error, TEXT("No tile found"));

	if (SelectedTile->GetTileStatus() == ETileStatus::EMPTY)
	{
		SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED);
		Field->TileMarked.Add(SelectedTile);
	}
}

void AChessPawn::MarkToCapture(int32 x, int32 y, int32 PlayerNumber, bool IsHumanPlayer)
{
	ATile* SelectedTile = nullptr;
	AGameField* Field = GMode->GField;

	GMode->CriticalSection.Lock();

	SelectedTile = Field->TileMap[FVector2D(x, y)];

	if (SelectedTile == nullptr)
		UE_LOG(LogTemp, Error, TEXT("No tile found"));

	if (SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
	{
		AChessPieces* SelectedPiece = Field->PiecesMap[FVector2D(x, y)];
		if (SelectedPiece == nullptr)
			UE_LOG(LogTemp, Error, TEXT("No piece found"));


		if (SelectedPiece->Color == (IsHumanPlayer ? EPieceColor::BLACK : EPieceColor::WHITE))
		{
			SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED_TO_CAPTURE);
			Field->TileMarked.Add(SelectedTile);
		}
	}
	GMode->CriticalSection.Unlock();
}
