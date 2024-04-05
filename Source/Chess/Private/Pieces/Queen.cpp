// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Pieces/Queen.h"
#include "../Public/Manager/Chess_GameMode.h"

AQueen::AQueen()
{
	Value = 9;

	Name = "Q";
}

bool AQueen::LegalMove(FBoard& Board, int32 PlayerNumber, bool CheckFlag)
{
	// Get the coordinates of the Queen
	int32 x = PieceGridPosition.X;
	int32 XMove = 0;
	int32 y = PieceGridPosition.Y;
	int32 YMove = 0;
	bool MarkedForward = false;
	bool MarkedBackwards = false;

	bool IsHumanPlayer = PlayerNumber == Player::Player1 ? true : false;

	XMove = IsHumanPlayer ? 1 : -1;
	YMove = IsHumanPlayer ? 1 : -1;

	// Calculation of legal moves along the line x = y
	for (int32 k = 0; k < 8; k++)
	{
		if (CheckCoord(x + XMove, y + YMove) && !MarkedForward)
		{
			if (!CheckFlag)
				MarkTile(Board, x + XMove, y + YMove, PlayerNumber, MarkedForward);
			else
				if (TestCheck(Board, x + XMove, y + YMove, PlayerNumber, MarkedForward))
					return true;
		}

		if (CheckCoord(x - XMove, y - YMove) && !MarkedBackwards)
		{
			if (!CheckFlag)
				MarkTile(Board, x - XMove, y - YMove, PlayerNumber, MarkedBackwards);
			else
				if (TestCheck(Board, x - XMove, y - YMove, PlayerNumber, MarkedBackwards))
					return true;
		}

		IsHumanPlayer ? XMove++ : XMove--;
		IsHumanPlayer ? YMove++ : YMove--;
	}
	
	XMove = IsHumanPlayer ? 1 : -1;
	YMove = IsHumanPlayer ? 1 : -1;
	MarkedForward = false;
	MarkedBackwards = false;

	// Calculation of legal moves along the line x = -y
	for (int32 k = 0; k < 8; k++)
	{
		if (CheckCoord(x + XMove, y - YMove) && !MarkedForward)
		{
			if (!CheckFlag)
				MarkTile(Board, x + XMove, y - YMove, PlayerNumber, MarkedForward);
			else
				if (TestCheck(Board, x + XMove, y - YMove, PlayerNumber, MarkedForward))
					return true;
		}

		if (CheckCoord(x - XMove, y + YMove) && !MarkedBackwards)
		{
			if (!CheckFlag)
				MarkTile(Board, x - XMove, y + YMove, PlayerNumber, MarkedBackwards);
			else
				if (TestCheck(Board, x - XMove, y + YMove, PlayerNumber, MarkedBackwards))
					return true;
		}

		IsHumanPlayer ? XMove++ : XMove--;
		IsHumanPlayer ? YMove++ : YMove--;
	}


	XMove = IsHumanPlayer ? 1 : -1;
	MarkedForward = false;
	MarkedBackwards = false;

	// Calculation of vertical moves
	for (int32 k = 0; k < 8; k++)
	{
		if (CheckCoord(x + XMove, y) && !MarkedForward)
		{
			if (!CheckFlag)
				MarkTile(Board, x + XMove, y, PlayerNumber, MarkedForward);
			else
				if (TestCheck(Board, x + XMove, y, PlayerNumber, MarkedForward))
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

	// calculation of horizontal moves
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