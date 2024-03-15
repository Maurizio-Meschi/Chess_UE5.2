// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Pieces/King.h"
#include "../Public/Manager/Chess_GameMode.h"

AKing::AKing()
{
	Name = "K";

	Value = 0;
}

bool AKing::LegalMove(int32 PlayerNumber, bool CheckFlag)
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

	if (CheckCoord(x + XMove, y + YMove) && !MarkedForward)
	{
		if (!CheckFlag)
			MarkTile(x + XMove, y + YMove, PlayerNumber, MarkedForward);
		else
			if (TestCheck(x + XMove, y + YMove, PlayerNumber, MarkedForward))
				return true;
	}

	if (CheckCoord(x - XMove, y - YMove) && !MarkedBackwards)
	{
		if (!CheckFlag)
			MarkTile(x - XMove, y - YMove, PlayerNumber, MarkedBackwards);
		else
			if (TestCheck(x - XMove, y - YMove, PlayerNumber, MarkedBackwards))
				return true;
	}

	XMove = IsHumanPlayer ? 1 : -1;
	YMove = IsHumanPlayer ? 1 : -1;
	MarkedForward = false;
	MarkedBackwards = false;

	if (CheckCoord(x + XMove, y - YMove) && !MarkedForward)
	{
		if (!CheckFlag)
			MarkTile(x + XMove, y - YMove, PlayerNumber, MarkedForward);
		else
			if (TestCheck(x + XMove, y - YMove, PlayerNumber, MarkedForward))
				return true;
	}

	if (CheckCoord(x - XMove, y + YMove) && !MarkedBackwards)
	{
		if (!CheckFlag)
			MarkTile(x - XMove, y + YMove, PlayerNumber, MarkedBackwards);
		else
			if (TestCheck(x - XMove, y + YMove, PlayerNumber, MarkedBackwards))
				return true;
	}

	XMove = IsHumanPlayer ? 1 : -1;
	MarkedForward = false;
	MarkedBackwards = false;

	if (CheckCoord(x + XMove, y) && !MarkedForward)
	{
		if (!CheckFlag)
			MarkTile(x + XMove, y, PlayerNumber, MarkedForward);
		else
			if (TestCheck(x + XMove, y, PlayerNumber, MarkedForward))
				return true;
	}

	if (CheckCoord(x - XMove, y) && !MarkedBackwards)
	{
		if (!CheckFlag)
			MarkTile(x - XMove, y, PlayerNumber, MarkedBackwards);
		else
			if (TestCheck(x - XMove, y, PlayerNumber, MarkedBackwards))
				return true;
	}
	
	YMove = IsHumanPlayer ? 1 : -1;
	MarkedForward = false;
	MarkedBackwards = false;

	if (CheckCoord(x, y + YMove) && !MarkedForward)
	{
		if (!CheckFlag)
			MarkTile(x, y + YMove, PlayerNumber, MarkedForward);
		else
			if (TestCheck(x, y + YMove, PlayerNumber, MarkedForward))
				return true;
	}

	if (CheckCoord(x, y - YMove) && !MarkedBackwards)
	{
		if (!CheckFlag)
			MarkTile(x, y - YMove, PlayerNumber, MarkedBackwards);
		else
			if (TestCheck(x, y - YMove, PlayerNumber, MarkedBackwards))
				return true;
	}
	return false;
}