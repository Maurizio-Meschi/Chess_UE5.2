// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Pieces/King.h"
#include "../Public/Manager/Chess_GameMode.h"

AKing::AKing()
{
	Name = "K";
}

void AKing::LegalMove(int32 PlayerNumber, bool IsHumanPlayer)
{
	FVector2D ChessPawnXYposition = PieceGridPosition;
	int32 x = ChessPawnXYposition.X;
	int32 XMove = 0;
	int32 y = ChessPawnXYposition.Y;
	int32 YMove = 0;
	bool MarkedForward = false;
	bool MarkedBackwards = false;

	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null King"));
		return;
	}

	AGameField* Field = GMode->GField;
	if (!Field)
	{
		UE_LOG(LogTemp, Error, TEXT("Field null King"));
		return;
	}

	XMove = IsHumanPlayer ? 1 : -1;
	YMove = IsHumanPlayer ? 1 : -1;

	if (CheckCoord(x + XMove, y + YMove) && !MarkedForward)
	{
		if (Field->CheckSituation)
			CheckMateSituation(x + XMove, y + YMove, PlayerNumber, IsHumanPlayer, MarkedForward);
		else
			Mark(x + XMove, y + YMove, PlayerNumber, IsHumanPlayer, MarkedForward);
	}

	if (CheckCoord(x - XMove, y - YMove) && !MarkedBackwards)
	{
		if (Field->CheckSituation)
			CheckMateSituation(x - XMove, y - YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);
		else
			Mark(x - XMove, y - YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);
	}

	XMove = IsHumanPlayer ? 1 : -1;
	YMove = IsHumanPlayer ? 1 : -1;
	MarkedForward = false;
	MarkedBackwards = false;

	if (CheckCoord(x + XMove, y - YMove) && !MarkedForward)
	{
		if (Field->CheckSituation)
			CheckMateSituation(x + XMove, y - YMove, PlayerNumber, IsHumanPlayer, MarkedForward);
		else
			Mark(x + XMove, y - YMove, PlayerNumber, IsHumanPlayer, MarkedForward);
	}

	if (CheckCoord(x - XMove, y + YMove) && !MarkedBackwards)
	{
		if (Field->CheckSituation)
			CheckMateSituation(x - XMove, y + YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);
		else
			Mark(x - XMove, y + YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);
	}

	XMove = IsHumanPlayer ? 1 : -1;
	MarkedForward = false;
	MarkedBackwards = false;

	if (CheckCoord(x + XMove, y) && !MarkedForward)
	{
		if (Field->CheckSituation)
			CheckMateSituation(x + XMove, y, PlayerNumber, IsHumanPlayer, MarkedForward);
		else
			Mark(x + XMove, y, PlayerNumber, IsHumanPlayer, MarkedForward);
	}

	if (CheckCoord(x - XMove, y) && !MarkedBackwards)
	{
		if (Field->CheckSituation)
			CheckMateSituation(x - XMove, y, PlayerNumber, IsHumanPlayer, MarkedBackwards);
		else
			Mark(x - XMove, y, PlayerNumber, IsHumanPlayer, MarkedBackwards);
	}
	
	YMove = IsHumanPlayer ? 1 : -1;
	MarkedForward = false;
	MarkedBackwards = false;

	if (CheckCoord(x, y + YMove) && !MarkedForward)
	{
		if (Field->CheckSituation)
			CheckMateSituation(x, y + YMove, PlayerNumber, IsHumanPlayer, MarkedForward);
		else
			Mark(x, y + YMove, PlayerNumber, IsHumanPlayer, MarkedForward);
	}

	if (CheckCoord(x, y - YMove) && !MarkedBackwards)
	{
		if (Field->CheckSituation)
			CheckMateSituation(x, y - YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);
		else
			Mark(x, y - YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);
	}
}

bool AKing::CheckKingSituation(int32 x, int32 y, bool IsHumanPlayer)
{
	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null King"));
		return false;
	}

	AGameField* Field = GMode->GField;
	if (!Field)
	{
		UE_LOG(LogTemp, Error, TEXT("Field null King"));
		return false;
	}

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

	ATile* SelectedTile = nullptr;

	
	if (TileMap.Contains(FVector2D(x, y)))
		SelectedTile = TileMap[FVector2D(x, y)];
	

	if (SelectedTile && SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
	{
		AChessPieces* SelectedPiece = nullptr;
		
		if (PiecesMap.Contains(FVector2D(x, y)))
			SelectedPiece = PiecesMap[FVector2D(x, y)];
		

		if (SelectedPiece && SelectedPiece->Color == (IsHumanPlayer ? EPieceColor::BLACK : EPieceColor::WHITE))
		{
			return true;
		}
	}
	return false;
}
