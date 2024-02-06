// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Pieces/King.h"
#include "Chess_GameMode.h"

AKing::AKing() {}

void AKing::LegalMove(int32 PlayerNumber, bool IsHumanPlayer)
{
	FVector2D ChessPawnXYposition = PieceGridPosition;
	int32 x = ChessPawnXYposition.X;
	int32 XMove = 0;
	int32 y = ChessPawnXYposition.Y;
	int32 YMove = 0;
	bool MarkedForward = false;
	bool MarkedBackwards = false;

	XMove = IsHumanPlayer ? 1 : -1;
	YMove = IsHumanPlayer ? 1 : -1;

	if (CheckCoord(x + XMove, y + YMove) && !MarkedForward)
		Mark(x + XMove, y + YMove, PlayerNumber, IsHumanPlayer, MarkedForward);

	if (CheckCoord(x - XMove, y - YMove) && !MarkedBackwards)
		Mark(x - XMove, y - YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);

	XMove = IsHumanPlayer ? 1 : -1;
	YMove = IsHumanPlayer ? 1 : -1;
	MarkedForward = false;
	MarkedBackwards = false;

	if (CheckCoord(x + XMove, y - YMove) && !MarkedForward)
		Mark(x + XMove, y - YMove, PlayerNumber, IsHumanPlayer, MarkedForward);

	if (CheckCoord(x - XMove, y + YMove) && !MarkedBackwards)
		Mark(x - XMove, y + YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);

	XMove = IsHumanPlayer ? 1 : -1;
	MarkedForward = false;
	MarkedBackwards = false;

	if (CheckCoord(x + XMove, y) && !MarkedForward)
		Mark(x + XMove, y, PlayerNumber, IsHumanPlayer, MarkedForward);

	if (CheckCoord(x - XMove, y) && !MarkedBackwards)
		Mark(x - XMove, y, PlayerNumber, IsHumanPlayer, MarkedBackwards);
	
	YMove = IsHumanPlayer ? 1 : -1;
	MarkedForward = false;
	MarkedBackwards = false;

	if (CheckCoord(x, y + YMove) && !MarkedForward)
		Mark(x, y + YMove, PlayerNumber, IsHumanPlayer, MarkedForward);

	if (CheckCoord(x, y - YMove) && !MarkedBackwards)
		Mark(x, y - YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);
}

bool AKing::CheckKingSituation(int32 x, int32 y, bool IsHumanPlayer)
{
	if (GameModeClass != nullptr)
		GMode = Cast<AChess_GameMode>(GWorld->GetAuthGameMode());
	else
		UE_LOG(LogTemp, Error, TEXT("Game Mode is null"));

	AGameField* Field = GMode->GField;

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

	ATile* SelectedTile = nullptr;

	GMode->CriticalSection.Lock();

	SelectedTile = TileMap[FVector2D(x, y)];

	GMode->CriticalSection.Unlock();

	if (SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
	{
		GMode->CriticalSection.Lock();

		AChessPieces* SelectedPiece = PiecesMap[FVector2D(x, y)];

		GMode->CriticalSection.Unlock();

		if (SelectedPiece->Color == (IsHumanPlayer ? EPieceColor::BLACK : EPieceColor::WHITE))
		{
			return true;
		}
	}
	return false;
}
