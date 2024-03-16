// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Pieces/ChessPawn.h"
#include "../../Public/MainActor/GameField.h"
#include "../Public/Manager/Chess_GameMode.h"

AChessPawn::AChessPawn()
{
	Value = 1;

	CaptureSituation = false;
}

bool AChessPawn::LegalMove(FBoard& Board, int32 PlayerNumber, bool CheckFlag)
{
	// get the coordinates of the pawn
	FVector2D ChessPawnXYposition = PieceGridPosition;
	int32 x = ChessPawnXYposition.X;
	int32 XMove = 0;
	int32 y = ChessPawnXYposition.Y;
	int32 YMove = 0;

	bool MarkedForward = false;

	bool IsHumanPlayer = PlayerNumber == 0 ? true : false;
	//UE_LOG(LogTemp, Error, TEXT("Prima chiamata alla legalmove di %s con CheckFalg = %s"), *this->GetName(), CheckFlag ? TEXT("TRUE") : TEXT("FALSE"));

	// check if the next first vertical tile is empty and if true, mark the tile
	XMove = IsHumanPlayer ? 1 : -1;
	if (CheckCoord(x + XMove, y))
	{
		CaptureSituation = false;

		if (!CheckFlag)
			MarkTile(Board, x + XMove, y, PlayerNumber, MarkedForward);
		else
			if (TestCheck(Board, x + XMove, y, PlayerNumber, MarkedForward))
				return true;
	}

	// check if the next second vertical tile is empty and if true, mark the tile
	if (x == (IsHumanPlayer? 1:6))
	{
		CaptureSituation = false;

		XMove = IsHumanPlayer ? 2 : -2;
		if (CheckCoord(x + XMove, y) && !MarkedForward)
		{
			if (!CheckFlag)
				MarkTile(Board, x + XMove, y, PlayerNumber, MarkedForward);
			else
				if (TestCheck(Board, x + XMove, y, PlayerNumber, MarkedForward))
					return true;
		}
	}

	// check if it is possible to capture an enemy piece
	int32 i = 1;
	for (int32 k = 0; k < 2; k++)
	{
		CaptureSituation = true;

		XMove = IsHumanPlayer ? 1 : -1;
		YMove = IsHumanPlayer ? i : -i;
		if (CheckCoord(x + XMove, y + YMove))
		{
			if (!CheckFlag)
				MarkTile(Board, x + XMove, y + YMove, PlayerNumber, MarkedForward);
			else
				if (TestCheck(Board, x + XMove, y + YMove, PlayerNumber, MarkedForward))
					return true;
		}
		i = -1;
	}
	
	return false;
}