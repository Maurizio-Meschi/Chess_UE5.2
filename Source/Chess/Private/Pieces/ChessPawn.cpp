// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Pieces/ChessPawn.h"
#include "../../Public/GameField.h"
#include "../Public/Chess_GameMode.h"

AChessPawn::AChessPawn()
{
	Value = 1;

	CaptureSituation = false;
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

	bool MarkedForward = false;

	if (GameModeClass != nullptr) 
		GMode = Cast<AChess_GameMode>(GWorld->GetAuthGameMode());
	else
		UE_LOG(LogTemp, Error, TEXT("Game Mode is null"));
	AGameField* Field = GMode->GField;

	// check if the next first vertical tile is empty and if true, mark the tile
	XMove = IsHumanPlayer ? 1 : -1;
	if (CheckCoord(x + XMove, y)) 
	{
		CaptureSituation = false;

		if (Field->CheckSituation)
			CheckMateSituationPawn(x + XMove, y, PlayerNumber, IsHumanPlayer);
		else
			MarkTile(x + XMove, y, PlayerNumber);
	}

	// check if it is possible to capture an enemy piece
	int32 i = 1;
	for (int32 k = 0; k < 2; k++)
	{
		XMove = IsHumanPlayer ? 1 : -1;
		YMove = IsHumanPlayer ? i : -i;
		if (CheckCoord(x + XMove, y + YMove))
		{
			CaptureSituation = true;

			if (Field->CheckSituation)
				CheckMateSituationPawn(x + XMove, y + YMove, PlayerNumber, IsHumanPlayer);
			else
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
			CaptureSituation = false;

			if (Field->CheckSituation)
				CheckMateSituationPawn(x + XMove, y, PlayerNumber, IsHumanPlayer);
			else
				MarkTile(x + XMove, y, PlayerNumber);
		}
	}
}

void AChessPawn::CheckMateSituationPawn(int32 x, int32 y, int32 PlayerNumber, bool IsHumanPlayer)
{
	ATile* SelectedTile = nullptr;
	AGameField* Field = GMode->GField;

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

	GMode->CriticalSection.Lock();

	SelectedTile = TileMap[FVector2D(x, y)];

	GMode->CriticalSection.Unlock();

	if (Field->IsCheckmateSituation)
	{
		if (SelectedTile->GetStatusCheckmate() == EStatusCheckmate::MARK_TO_AVOID_CHECKMATE ||
			SelectedTile->GetStatusCheckmate() == EStatusCheckmate::MARK_BY_KING)
		{
			SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED);
			Field->AddTileMarked(SelectedTile);
		}

		if (SelectedTile->GetStatusCheckmate() == EStatusCheckmate::CAPTURE_TO_AVOID_CHECKMATE)
		{
			SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED_TO_CAPTURE);
			Field->AddTileMarked(SelectedTile);
		}
	}
	else
	{
		if (CaptureSituation && SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
		{
			GMode->CriticalSection.Lock();
			AChessPieces* SelectedPiece = PiecesMap[FVector2D(x, y)];
			GMode->CriticalSection.Unlock();
			if (SelectedPiece == nullptr)
				UE_LOG(LogTemp, Error, TEXT("No piece found"));


			if (SelectedPiece->Color == (IsHumanPlayer ? EPieceColor::BLACK : EPieceColor::WHITE))
			{
				if (SelectedPiece->GetClass()->GetName() == (IsHumanPlayer ? "BP_b_King_C" : "BP_w_King_C"))
				{
					Field->KingUnderAttack = true;
					FVector2d PiecePosition = this->GetGridPosition();
					GMode->CriticalSection.Lock();
					TileMap[PiecePosition]->SEtStatusCheckmate(PlayerNumber, EStatusCheckmate::CAPTURE_TO_AVOID_CHECKMATE);
					GMode->CriticalSection.Unlock();
					// Marca le tile per arrivare al re
					FindTileBetweenP1P2(PiecePosition, FVector2D(x, y), PlayerNumber);
				}
			}
		}
		if (SelectedTile->GetStatusCheckmate() == EStatusCheckmate::MARK_BY_KING)
		{
			SelectedTile->SEtStatusCheckmate(PlayerNumber, EStatusCheckmate::BLOCK_KING);;
		}
	}
}

void AChessPawn::MarkToCapture(int32 x, int32 y, int32 PlayerNumber, bool IsHumanPlayer)
{
	ATile* SelectedTile = nullptr;
	AGameField* Field = GMode->GField;

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

	GMode->CriticalSection.Lock();
	SelectedTile = TileMap[FVector2D(x, y)];
	GMode->CriticalSection.Unlock();

	if (SelectedTile == nullptr)
		UE_LOG(LogTemp, Error, TEXT("No tile found"));

	if (SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
	{
		GMode->CriticalSection.Lock();
		AChessPieces* SelectedPiece = PiecesMap[FVector2D(x, y)];
		GMode->CriticalSection.Unlock();
		if (SelectedPiece == nullptr)
			UE_LOG(LogTemp, Error, TEXT("No piece found"));


		if (SelectedPiece->Color == (IsHumanPlayer ? EPieceColor::BLACK : EPieceColor::WHITE))
		{
			if (SelectedPiece->GetClass()->GetName() != (IsHumanPlayer ? "BP_b_King_C" : "BP_w_King_C"))
			{
				SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED_TO_CAPTURE);
				Field->AddTileMarked(SelectedTile);
			}
		}
	}
}

void AChessPawn::MarkTile(int32 x, int32 y, int32 PlayerNumber)
{
	ATile* SelectedTile = nullptr;
	AGameField* Field = GMode->GField;

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

	GMode->CriticalSection.Lock();

	SelectedTile = TileMap[FVector2D(x, y)];

	GMode->CriticalSection.Unlock();

	if (SelectedTile == nullptr)
		UE_LOG(LogTemp, Error, TEXT("No tile found"));

	if (SelectedTile->GetTileStatus() == ETileStatus::EMPTY)
	{
		SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED);
		Field->AddTileMarked(SelectedTile);
	}
}

