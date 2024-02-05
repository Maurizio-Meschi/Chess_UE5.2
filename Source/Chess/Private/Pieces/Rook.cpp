// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Pieces/Rook.h"
#include "../Public/Chess_GameMode.h"


ARook::ARook() 
{
	Value = 5;
}

void ARook::LegalMove(int32 PlayerNumber, bool IsHumanPlayer)
{
	FVector2D ChessPawnXYposition = PieceGridPosition;
	int32 x = ChessPawnXYposition.X;
	int32 XMove = 0;
	int32 y = ChessPawnXYposition.Y;
	int32 YMove = 0;
	bool MarkedForward = false;
	bool MarkedBackwards = false;

	XMove   = IsHumanPlayer ? 1 : -1;


	for (int32 k = 0; k < 8; k++)
	{
		if (CheckCoord(x + XMove, y) && !MarkedForward)
			Mark(x + XMove, y, PlayerNumber, IsHumanPlayer, MarkedForward);

		if (CheckCoord(x - XMove, y) && !MarkedBackwards)
			Mark(x - XMove, y, PlayerNumber, IsHumanPlayer, MarkedBackwards);

		IsHumanPlayer ? XMove++ : XMove--;
	}

	YMove = IsHumanPlayer ? 1 : -1;
	MarkedForward = false;
	MarkedBackwards = false;
	for (int32 k = 0; k < 8; k++)
	{
		if (CheckCoord(x, y + YMove) && !MarkedForward)
			Mark(x, y + YMove, PlayerNumber, IsHumanPlayer, MarkedForward);

		if (CheckCoord(x, y - YMove) && !MarkedBackwards)
			Mark(x, y - YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);

		if (MarkedForward && MarkedBackwards) break;

		IsHumanPlayer ? YMove++ : YMove--;
	}
}

