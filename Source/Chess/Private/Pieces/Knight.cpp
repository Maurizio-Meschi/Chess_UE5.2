// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Pieces/Knight.h"
#include "../Public/Chess_GameMode.h"

AKnight::AKnight()
{
	Value = 3;
}

void AKnight ::LegalMove(int32 PlayerNumber, bool IsHumanPlayer)
{
	FVector2D ChessPawnXYposition = PieceGridPosition;
	int32 x = ChessPawnXYposition.X;
	int32 XMove = 0;
	int32 y = ChessPawnXYposition.Y;
	int32 YMove = 0;
	bool MarkedForward = false;
	bool MarkedBackwards = false;

	XMove = IsHumanPlayer ? 1 : -1;
	YMove = IsHumanPlayer ? 2 : -2;

	for (int32 k = 0; k < 2; k++)
	{
		if (CheckCoord(x + XMove, y + YMove) && !MarkedForward)
			Mark(x + XMove, y + YMove, PlayerNumber, IsHumanPlayer, MarkedForward);

		if (CheckCoord(x - XMove, y - YMove) && !MarkedBackwards)
			Mark(x - XMove, y - YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);

		XMove = IsHumanPlayer ? 2 : -2;
		YMove = IsHumanPlayer ? 1 : -1;
		MarkedForward = false;
		MarkedBackwards = false;
	}

	XMove = IsHumanPlayer ? 1 : -1;
	YMove = IsHumanPlayer ? 2 : -2;

	for (int32 k = 0; k < 2; k++)
	{
		if (CheckCoord(x + XMove, y - YMove) && !MarkedForward)
			Mark(x + XMove, y - YMove, PlayerNumber, IsHumanPlayer, MarkedForward);

		if (CheckCoord(x - XMove, y + YMove) && !MarkedBackwards)
			Mark(x - XMove, y + YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);

		XMove = IsHumanPlayer ? 2 : -2;
		YMove = IsHumanPlayer ? 1 : -1;
		MarkedForward = false;
		MarkedBackwards = false;
	}
}