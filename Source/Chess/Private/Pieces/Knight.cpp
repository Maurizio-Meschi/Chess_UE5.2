// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Pieces/Knight.h"
#include "../Public/Chess_GameMode.h"

AKnight::AKnight()
{
	Value = 3;

	Name = "N";
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

	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null Knight"));
		return;
	}

	AGameField* Field = GMode->GField;
	if (!Field)
	{
		UE_LOG(LogTemp, Error, TEXT("Field null Knight"));
		return;
	}

	XMove = IsHumanPlayer ? 1 : -1;
	YMove = IsHumanPlayer ? 2 : -2;

	for (int32 k = 0; k < 2; k++)
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

		XMove = IsHumanPlayer ? 2 : -2;
		YMove = IsHumanPlayer ? 1 : -1;
		MarkedForward = false;
		MarkedBackwards = false;
	}
}