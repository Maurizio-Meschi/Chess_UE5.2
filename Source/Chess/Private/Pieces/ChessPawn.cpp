// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Pieces/ChessPawn.h"
#include "../../Public/MainActor/GameField.h"
#include "../Public/Manager/Chess_GameMode.h"

AChessPawn::AChessPawn()
{
	Value = 1;

	CaptureSituation = false;
}

void AChessPawn::LegalMove(int32 PlayerNumber, bool IsHumanPlayer)
{
	// get the coordinates of the pawn
	FVector2D ChessPawnXYposition = PieceGridPosition;
	int32 x = ChessPawnXYposition.X;
	int32 XMove = 0;
	int32 y = ChessPawnXYposition.Y;
	int32 YMove = 0;

	bool MarkedForward = false;

	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null Pawn"));
		return;
	}

	AGameField* Field = GMode->GField;
	if (!Field)
	{
		UE_LOG(LogTemp, Error, TEXT("Field null Pawn"));
		return;
	}

	// check if the next first vertical tile is empty and if true, mark the tile
	XMove = IsHumanPlayer ? 1 : -1;
	if (CheckCoord(x + XMove, y) && !MarkedForward)
	{
		CaptureSituation = false;

		if (Field->CheckSituation)
			CheckMateSituationPawn(x + XMove, y, PlayerNumber, IsHumanPlayer);
		else
			MarkTile(x + XMove, y, PlayerNumber, MarkedForward);
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
		if (CheckCoord(x + XMove, y) && !MarkedForward)
		{
			CaptureSituation = false;

			if (Field->CheckSituation)
				CheckMateSituationPawn(x + XMove, y, PlayerNumber, IsHumanPlayer);
			else
				MarkTile(x + XMove, y, PlayerNumber, MarkedForward);
		}
	}
}

void AChessPawn::CheckMateSituationPawn(int32 x, int32 y, int32 PlayerNumber, bool IsHumanPlayer)
{
	ATile* SelectedTile = nullptr;

	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null Pawn"));
		return;
	}

	AGameField* Field = GMode->GField;
	if (!Field)
	{
		UE_LOG(LogTemp, Error, TEXT("Field null Pawn"));
		return;
	}

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

	
	if (TileMap.Contains(FVector2D(x, y)))
		SelectedTile = TileMap[FVector2D(x, y)];
	

	if (Field->IsCheckmateSituation && SelectedTile)
	{
		ManagerCheckMateSituationPawn(SelectedTile, PlayerNumber);
	}
	else
	{
		if (CaptureSituation && SelectedTile && SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
		{
			AChessPieces* SelectedPiece = nullptr;

			
			if (PiecesMap.Contains(FVector2D(x, y)))
				SelectedPiece = PiecesMap[FVector2D(x, y)];
			

			if (SelectedPiece && SelectedPiece->Color == (IsHumanPlayer ? EPieceColor::BLACK : EPieceColor::WHITE))
			{
				if (SelectedPiece->IsA<AKing>())
				{
					Field->KingUnderAttack = true;
				}
			}
		}
		if (CaptureSituation && SelectedTile->GetStatusCheckmate() == EStatusCheckmate::MARK_BY_KING || 
			SelectedTile->GetStatusCheckmate() == EStatusCheckmate::CAPTURE_BY_KING ||
			SelectedTile->GetStatusCheckmate() == EStatusCheckmate::CAPTURE_TO_AVOID_CHECKMATE)
		{
			SelectedTile->SetStatusCheckmate(PlayerNumber, EStatusCheckmate::BLOCK_KING);;
		}
	}
}

void AChessPawn::ManagerCheckMateSituationPawn(ATile* SelectedTile, int32 PlayerNumber)
{
	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null Pawn"));
		return;
	}

	AGameField* Field = GMode->GField;
	if (!Field)
	{
		UE_LOG(LogTemp, Error, TEXT("Field null Pawn"));
		return;
	}

	if (!SelectedTile)
	{
		UE_LOG(LogTemp, Error, TEXT("SelectedTile null Pawn"));
		return;
	}

	if (!CaptureSituation && 
		(SelectedTile->GetStatusCheckmate() == EStatusCheckmate::MARK_TO_AVOID_CHECKMATE ||
		SelectedTile->GetStatusCheckmate() == EStatusCheckmate::MARK_AND_BLOCK_KING))
	{
		SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED);
		Field->AddTileMarked(SelectedTile);
	}

	if (CaptureSituation && SelectedTile->GetStatusCheckmate() == EStatusCheckmate::CAPTURE_TO_AVOID_CHECKMATE)
	{
		SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED_TO_CAPTURE);
		Field->AddTileMarked(SelectedTile);
	}
}

void AChessPawn::MarkToCapture(int32 x, int32 y, int32 PlayerNumber, bool IsHumanPlayer)
{
	ATile* SelectedTile = nullptr;

	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null Pawn"));
		return;
	}

	AGameField* Field = GMode->GField;
	if (!Field)
	{
		UE_LOG(LogTemp, Error, TEXT("Field null Pawn"));
		return;
	}

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

	
	if (TileMap.Contains(FVector2D(x, y)))
		SelectedTile = TileMap[FVector2D(x, y)];
	

	if (SelectedTile && SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
	{
		AChessPieces* SelectedPiece = nullptr;

		
		if (PiecesMap.Contains(FVector2D(x, y)))
			SelectedPiece = PiecesMap[FVector2D(x, y)];
		
		
		if (SelectedPiece && SelectedPiece->Color == (IsHumanPlayer ? EPieceColor::BLACK : EPieceColor::WHITE))
		{
			if (!SelectedPiece->IsA<AKing>())
			{
				SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED_TO_CAPTURE);
				Field->AddTileMarked(SelectedTile);
			}
		}
	}
}

void AChessPawn::MarkTile(int32 x, int32 y, int32 PlayerNumber, bool &Marked)
{
	ATile* SelectedTile = nullptr;

	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null Pawn"));
		return;
	}

	AGameField* Field = GMode->GField;
	if (!Field)
	{
		UE_LOG(LogTemp, Error, TEXT("Field null Pawn"));
		return;
	}

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

	
	if (TileMap.Contains(FVector2D(x, y)))
		SelectedTile = TileMap[FVector2D(x, y)];
	

	if (SelectedTile && SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
	{
		Marked = true;
	}

	if (SelectedTile && SelectedTile->GetTileStatus() == ETileStatus::EMPTY)
	{
		SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED);
		Field->AddTileMarked(SelectedTile);
	}
}

