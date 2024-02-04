// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Pieces/Rook.h"
#include "../Public/Chess_GameMode.h"


ARook::ARook() 
{
	Value = 5;
}

void ARook::LegalMove(int32 PlayerNumber, bool IsHumanPlayer)
{
	FVector2D ChessPawnXYposition = PieceGridPosition;
	int32 x = ChessPawnXYposition.X;
	int32 XMove = 0;
	int32 y = ChessPawnXYposition.Y;
	int32 YMove = 0;
	bool MarkedForward = false;
	bool MarkedBackwards = false;

	if (GameModeClass != nullptr)
		GMode = Cast<AChess_GameMode>(GWorld->GetAuthGameMode());
	else
		UE_LOG(LogTemp, Error, TEXT("Game Mode is null"));

	AGameField* Field = GMode->GField;

	
	XMove   = IsHumanPlayer ? 1 : -1;


	for (int32 k = 0; k < 8; k++)
	{
		if (CheckCoord(x + XMove, y) && !MarkedForward)
			Mark(x + XMove, y, PlayerNumber, IsHumanPlayer, MarkedForward);

		if (CheckCoord(x - XMove, y) && !MarkedBackwards)
			Mark(x - XMove, y, PlayerNumber, IsHumanPlayer, MarkedBackwards);

		IsHumanPlayer ? XMove++ : XMove--;
	}

	YMove = IsHumanPlayer ? 1 : -1;
	MarkedForward = false;
	MarkedBackwards = false;
	for (int32 k = 0; k < 8; k++)
	{
		if (CheckCoord(x, y + YMove) && !MarkedForward)
			Mark(x, y + YMove, PlayerNumber, IsHumanPlayer, MarkedForward);

		if (CheckCoord(x, y - YMove) && !MarkedBackwards)
			Mark(x, y - YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);

		if (MarkedForward && MarkedBackwards) break;

		IsHumanPlayer ? YMove++ : YMove--;
	}
}

void ARook::Mark(int32 x, int32 y, int32 PlayerNumber, bool IsHumanPlayer, bool& Marked)
{
	if (GameModeClass != nullptr)
		GMode = Cast<AChess_GameMode>(GWorld->GetAuthGameMode());
	else
		UE_LOG(LogTemp, Error, TEXT("Game Mode is null"));

	AGameField* Field = GMode->GField;
	ATile* SelectedTile = nullptr;

	GMode->CriticalSection.Lock();

	SelectedTile = Field->TileMap[FVector2D(x, y)];

	GMode->CriticalSection.Unlock();

	if (SelectedTile->GetTileStatus() == ETileStatus::EMPTY)
	{
		SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED);
		Field->TileMarked.Add(SelectedTile);
	}
	else if (SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
	{
		GMode->CriticalSection.Lock();

		AChessPieces* SelectedPiece = Field->PiecesMap[FVector2D(x, y)];

		GMode->CriticalSection.Unlock();

		if (SelectedPiece->Color == (IsHumanPlayer ? EPieceColor::BLACK : EPieceColor::WHITE))
		{
			SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED_TO_CAPTURE);
			Field->TileMarked.Add(SelectedTile);
			Marked = true;
		}
		else
			Marked = true;
	}
}

