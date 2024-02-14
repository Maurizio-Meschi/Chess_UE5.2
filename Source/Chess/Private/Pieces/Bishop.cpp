// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Pieces/Bishop.h"
#include "../Public/Chess_GameMode.h"

ABishop::ABishop()
{
	Value = 3;
}

void ABishop::LegalMove(int32 PlayerNumber, bool IsHumanPlayer)
{
	FVector2D ChessPawnXYposition = PieceGridPosition;
	int32 x = ChessPawnXYposition.X;
	int32 XMove = 0;
	int32 y = ChessPawnXYposition.Y;
	int32 YMove = 0;
	bool MarkedForward = false;
	bool MarkedBackwards = false;

	AGameField* Field = GMode->GField;

	XMove = IsHumanPlayer ? 1 : -1;
	YMove = IsHumanPlayer ? 1 : -1;

	for (int32 k = 0; k < 8; k++)
	{
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

		IsHumanPlayer ? XMove++ : XMove--;
		IsHumanPlayer ? YMove++ : YMove--;
	}

	XMove = IsHumanPlayer ? 1 : -1;
	YMove = IsHumanPlayer ? 1 : -1;
	MarkedForward = false;
	MarkedBackwards = false;

	for (int32 k = 0; k < 8; k++)
	{
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

		IsHumanPlayer ? XMove++ : XMove--;
		IsHumanPlayer ? YMove++ : YMove--;
	}
}