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
	
	bool IsHumanPlayer = PlayerNumber == Player::Player1 ? true : false;

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
	auto Pieces = IsHumanPlayer ? GField->GetAIPieces() : GField->GetPlayer1Pieces();
	for (auto Piece : Pieces)
	{
		if (!Piece->IsA<AKing>() && Piece->LegalMove(Board, IsHumanPlayer ? Player::AI : Player::Player1, true))
			return false;
	}

	FMarked Obj;

	// Long castling
	auto LeftRookPos = IsHumanPlayer ? LEFT_HR_POSITION : LEFT_AIR_POSITION;
	FVector2D CastlingTilePos(LeftRookPos.X, LeftRookPos.Y + 1);
	FVector2D KingPos = IsHumanPlayer ? HK_POSITION : AIK_POSITION;
	FVector2D TileNextToKing(KingPos.X, KingPos.Y - 1);

	// Check if the tile left to the king is a legal move. Otherwise long castling is not possible
	if (Board.Field.Contains(TileNextToKing))
		Obj.Tile = Board.Field[TileNextToKing];

	if (NeverMoved && ManagerPiece->LegalMoveArray[IndexArray].Contains(Obj))
	{
		if (Board.Field.Contains(CastlingTilePos))
		{
			if (Board.Field[CastlingTilePos]->GetTileStatus() == ETileStatus::EMPTY)
			{
				CastlingTilePos.Y += 1;
				Castling(Board, CastlingTilePos, LeftRookPos, PlayerNumber, MarkedForward);
			}
		}
	}

	// Castling
	auto RightRookPos = IsHumanPlayer ? RIGHT_HR_POSITION : RIGHT_AIR_POSITION;
	CastlingTilePos.X = RightRookPos.X;
	CastlingTilePos.Y = RightRookPos.Y - 1;
	TileNextToKing.Y = KingPos.Y + 1;

	// Check if the tile right to the king is a legal move. Otherwise castling is not possible
	if (Board.Field.Contains(TileNextToKing))
		Obj.Tile = Board.Field[TileNextToKing];

	if (NeverMoved && ManagerPiece->LegalMoveArray[IndexArray].Contains(Obj))
	{
		if (Board.Field.Contains(CastlingTilePos))
		{
			if (Board.Field[CastlingTilePos]->GetTileStatus() == ETileStatus::EMPTY)
			{
				Castling(Board, CastlingTilePos, RightRookPos, PlayerNumber, MarkedForward);
			}
		}
	}

	return false;
}