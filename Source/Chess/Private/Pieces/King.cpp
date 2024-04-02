// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Pieces/King.h"
#include "../Public/Manager/Chess_GameMode.h"

AKing::AKing()
{
	Name = "K";

	Value = 0;
}

bool AKing::LegalMove(FBoard& Board, int32 PlayerNumber, bool CheckFlag)
{
	// Get the coordinates of the king
	int32 x = PieceGridPosition.X;
	int32 XMove = 0;
	int32 y = PieceGridPosition.Y;
	int32 YMove = 0;
	bool MarkedForward = false;
	
	bool IsHumanPlayer = PlayerNumber == Player::HUMAN ? true : false;

	// Calculate the king's legal moves
	for (int32 i = 0; i < 3; ++i)
	{
		for (int32 j = 0; j < 3; ++j)
		{
			if (i == 1 && j == 1)
				continue;

			XMove = i - 1;
			YMove = j - 1;

			if (CheckCoord(x + XMove, y + YMove)) 
			{
				if (!CheckFlag)
					MarkTile(Board, x + XMove, y + YMove, PlayerNumber, MarkedForward);
				else
					if (TestCheck(Board, x + XMove, y + YMove, PlayerNumber, MarkedForward))
						return true;
			}
		}
	}

	AChess_GameMode* GMode = nullptr;
	AGameField* GField = nullptr;
	AManagePiece* ManagerPiece = nullptr;

	if (!FGameRef::GetGameRef(this, GMode, GField, ManagerPiece, "King"))
		return false;

	// Before considering castling, check that the king is not in check
	auto Pieces = IsHumanPlayer ? GField->GetBotPieces() : GField->GetHumanPlayerPieces();
	for (auto Piece : Pieces)
	{
		if (!Piece->IsA<AKing>() && Piece->LegalMove(Board, IsHumanPlayer ? Player::AI : Player::HUMAN, true))
			return false;
	}

	FMarked Obj;

	// Human castling
	if (Board.Field.Contains(FVector2D(HK_POSITION.X,HK_POSITION.Y + 1)))
		Obj.Tile = Board.Field[FVector2D(HK_POSITION.X,HK_POSITION.Y + 1)];

	if (ManagerPiece->LegalMoveArray[IndexArray].Contains(Obj) && NeverMoved)
		Castling(Board, FVector2D(HR2_POSITION.X, HR2_POSITION.Y - 1), HR2_POSITION, PlayerNumber, MarkedForward);

	// Long human castling
	if (Board.Field.Contains(FVector2D(HK_POSITION.X,HK_POSITION.Y - 1)))
		Obj.Tile = Board.Field[FVector2D(HK_POSITION.X,HK_POSITION.Y - 1)];

	if (ManagerPiece->LegalMoveArray[IndexArray].Contains(Obj) && NeverMoved)
	{
		if (Board.Field.Contains(FVector2D(HR1_POSITION.X, HR1_POSITION.Y + 1)))
			if (Board.Field[FVector2D(HR1_POSITION.X, HR1_POSITION.Y + 1)]->GetTileStatus() == ETileStatus::EMPTY)
				Castling(Board, FVector2D(HR1_POSITION.X, HR1_POSITION.Y + 2), HR1_POSITION, PlayerNumber, MarkedForward);
	}

	// AI castling
	if (Board.Field.Contains(FVector2D(AIK_POSITION.X, AIK_POSITION.Y + 1)))
		Obj.Tile = Board.Field[FVector2D(AIK_POSITION.X, AIK_POSITION.Y + 1)];


	if (ManagerPiece->LegalMoveArray[IndexArray].Contains(Obj) && NeverMoved)
	{
		Castling(Board, FVector2D(AIR2_POSITION.X, AIR2_POSITION.Y - 1), AIR2_POSITION, PlayerNumber, MarkedForward);
	}

	// Long AI castling
	if (Board.Field.Contains(FVector2D(AIK_POSITION.X,HK_POSITION.Y - 1)))
		Obj.Tile = Board.Field[FVector2D(AIK_POSITION.X,HK_POSITION.Y - 1)];

	if (ManagerPiece->LegalMoveArray[IndexArray].Contains(Obj) && NeverMoved)
	{
		if (Board.Field.Contains(FVector2D(AIR1_POSITION.X, AIR1_POSITION.Y + 1)))
			if (Board.Field[FVector2D(AIR1_POSITION.X, AIR1_POSITION.Y + 1)]->GetTileStatus() == ETileStatus::EMPTY)
				Castling(Board, FVector2D(AIR1_POSITION.X, AIR1_POSITION.Y + 2), AIR1_POSITION, PlayerNumber, MarkedForward);
	}
		
	return false;
}