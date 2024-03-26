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
	FVector2D ChessPawnXYposition = PieceGridPosition;
	int32 x = ChessPawnXYposition.X;
	int32 XMove = 0;
	int32 y = ChessPawnXYposition.Y;
	int32 YMove = 0;
	bool MarkedForward = false;
	bool MarkedBackwards = false;

	bool IsHumanPlayer = PlayerNumber == 0 ? true : false;

	XMove = IsHumanPlayer ? 1 : -1;
	YMove = IsHumanPlayer ? 1 : -1;

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

	FMarked Obj;

	// Arrocco
	if (Board.Field.Contains(FVector2D(HUMAN_KING_POSITION.X, HUMAN_KING_POSITION.Y + 1)))
		Obj.Tile = Board.Field[FVector2D(HUMAN_KING_POSITION.X, HUMAN_KING_POSITION.Y + 1)];

	if (ManagerPiece->LegalMoveArray[IndexArray].Contains(Obj) && NeverMoved)
		Castling(Board, FVector2D(HUMAN_ROOK_POSITION2.X, HUMAN_ROOK_POSITION2.Y - 1), HUMAN_ROOK_POSITION2, PlayerNumber, MarkedForward);

	// Arrocco Lungo
	if (Board.Field.Contains(FVector2D(HUMAN_KING_POSITION.X, HUMAN_KING_POSITION.Y - 1)))
		Obj.Tile = Board.Field[FVector2D(HUMAN_KING_POSITION.X, HUMAN_KING_POSITION.Y - 1)];

	if (ManagerPiece->LegalMoveArray[IndexArray].Contains(Obj) && NeverMoved)
	{
		if (Board.Field.Contains(FVector2D(HUMAN_ROOK_POSITION1.X, HUMAN_ROOK_POSITION1.Y + 1)))
			if (Board.Field[FVector2D(HUMAN_ROOK_POSITION1.X, HUMAN_ROOK_POSITION1.Y + 1)]->GetTileStatus() == ETileStatus::EMPTY)
				Castling(Board, FVector2D(HUMAN_ROOK_POSITION1.X, HUMAN_ROOK_POSITION1.Y + 2), HUMAN_ROOK_POSITION1, PlayerNumber, MarkedForward);
	}

	// Arrocco Nemico
	if (Board.Field.Contains(FVector2D(AI_KING_POSITION.X, AI_KING_POSITION.Y + 1)))
		Obj.Tile = Board.Field[FVector2D(AI_KING_POSITION.X, AI_KING_POSITION.Y + 1)];


	if (ManagerPiece->LegalMoveArray[IndexArray].Contains(Obj) && NeverMoved)
	{
		Castling(Board, FVector2D(AI_ROOK_POSITION2.X, AI_ROOK_POSITION2.Y - 1), AI_ROOK_POSITION2, PlayerNumber, MarkedForward);
	}

	// Arrocco Lungo nemico
	if (Board.Field.Contains(FVector2D(AI_KING_POSITION.X, HUMAN_KING_POSITION.Y - 1)))
		Obj.Tile = Board.Field[FVector2D(AI_KING_POSITION.X, HUMAN_KING_POSITION.Y - 1)];

	if (ManagerPiece->LegalMoveArray[IndexArray].Contains(Obj) && NeverMoved)
	{
		if (Board.Field.Contains(FVector2D(AI_ROOK_POSITION1.X, AI_ROOK_POSITION1.Y + 1)))
			if (Board.Field[FVector2D(AI_ROOK_POSITION1.X, AI_ROOK_POSITION1.Y + 1)]->GetTileStatus() == ETileStatus::EMPTY)
				Castling(Board, FVector2D(AI_ROOK_POSITION1.X, AI_ROOK_POSITION1.Y + 2), AI_ROOK_POSITION1, PlayerNumber, MarkedForward);
	}
		
	return false;
}