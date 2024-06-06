// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Pieces/Rook.h"
#include "../Public/Manager/Chess_GameMode.h"


ARook::ARook() 
{
	Value = 477;// 50;

	EGValue = 512;

	Name = "R";
}

bool ARook::LegalMove(FBoard& Board, int32 PlayerNumber, bool CheckFlag)
{
	// Get the coordinates of the rook
	int32 x = PieceGridPosition.X;
	int32 XMove = 0;
	int32 y = PieceGridPosition.Y;
	int32 YMove = 0;
	bool MarkedForward = false;
	bool MarkedBackwards = false;

	bool IsHumanPlayer = PlayerNumber == 0 ? true : false;

	XMove   = IsHumanPlayer ? 1 : -1;

	// Calculation of vertical moves
	for (int32 k = 0; k < 8; k++)
	{
		if (CheckCoord(x + XMove, y) && !MarkedForward)
		{
			if (!CheckFlag)
				MarkTile(Board, x + XMove, y + YMove, PlayerNumber, MarkedForward);
			else
				if (TestCheck(Board, x + XMove, y + YMove, PlayerNumber, MarkedForward))
					return true;
		}
		if (CheckCoord(x - XMove, y) && !MarkedBackwards)
		{
			if (!CheckFlag)
				MarkTile(Board, x - XMove, y, PlayerNumber, MarkedBackwards);
			else
				if (TestCheck(Board, x - XMove, y, PlayerNumber, MarkedBackwards))
					return true;
		}

		IsHumanPlayer ? XMove++ : XMove--;
	}
	

	YMove = IsHumanPlayer ? 1 : -1;
	MarkedForward = false;
	MarkedBackwards = false;

	// Calculation of horizontal moves
	for (int32 k = 0; k < 8; k++)
	{
		if (CheckCoord(x, y + YMove) && !MarkedForward)
		{
			if (!CheckFlag)
				MarkTile(Board, x, y + YMove, PlayerNumber, MarkedForward);
			else
				if (TestCheck(Board, x, y + YMove, PlayerNumber, MarkedForward))
					return true;
		}

		if (CheckCoord(x, y - YMove) && !MarkedBackwards)
		{
			if (!CheckFlag)
				MarkTile(Board, x, y - YMove, PlayerNumber, MarkedBackwards);
			else
				if (TestCheck(Board, x, y - YMove, PlayerNumber, MarkedBackwards))
					return true;
		}

		if (MarkedForward && MarkedBackwards) break;

		IsHumanPlayer ? YMove++ : YMove--;
	}
	return false;
}

