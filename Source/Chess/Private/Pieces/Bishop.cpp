// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Pieces/Bishop.h"
#include "../Public/Manager/Chess_GameMode.h"

ABishop::ABishop()
{
	Value = 3;

	Name = "B";
}

bool ABishop::LegalMove(int32 PlayerNumber, bool CheckFlag)
{
	FVector2D ChessPawnXYposition = PieceGridPosition;
	int32 x = ChessPawnXYposition.X;
	int32 XMove = 0;
	int32 y = ChessPawnXYposition.Y;
	int32 YMove = 0;
	bool MarkedForward = false;
	bool MarkedBackwards = false;

	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null Pawn"));
		return false;
	}

	AGameField* Field = GMode->GField;
	if (!Field)
	{
		UE_LOG(LogTemp, Error, TEXT("Field null Pawn"));
		return false;
	}

	bool IsHumanPlayer = PlayerNumber == 0 ? true : false;

	XMove = IsHumanPlayer ? 1 : -1;
	YMove = IsHumanPlayer ? 1 : -1;

	for (int32 k = 0; k < 8; k++)
	{
		if (CheckCoord(x + XMove, y + YMove) && !MarkedForward)
		{
			if (!CheckFlag)
				MarkTile(x + XMove, y + YMove, PlayerNumber, MarkedForward);
			else
			{
				if (TestCheck(x + XMove, y + YMove, PlayerNumber, MarkedForward))
					return true;
			}
		}

		if (CheckCoord(x - XMove, y - YMove) && !MarkedBackwards)
		{
			if (!CheckFlag)
				MarkTile(x - XMove, y - YMove, PlayerNumber, MarkedBackwards);
			else
			{
				if (TestCheck(x - XMove, y - YMove, PlayerNumber, MarkedBackwards))
					return true;
			}
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
			if (!CheckFlag)
				MarkTile(x + XMove, y - YMove, PlayerNumber, MarkedForward);
			else
			{
				if (TestCheck(x + XMove, y - YMove, PlayerNumber, MarkedForward))
					return true;
			}
		}

		if (CheckCoord(x - XMove, y + YMove) && !MarkedBackwards)
		{
			if (!CheckFlag)
				MarkTile(x - XMove, y + YMove, PlayerNumber, MarkedBackwards);
			else
				if (TestCheck(x - XMove, y + YMove, PlayerNumber, MarkedBackwards))
					return true;
		}

		IsHumanPlayer ? XMove++ : XMove--;
		IsHumanPlayer ? YMove++ : YMove--;
	}

	return false;
}