// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/Pieces/Rook.h"
#include "../Public/Chess_GameMode.h"


ARook::ARook() 
{
	Value = 5;

	Name = "R";
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

	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null Rook"));
		return;
	}

	AGameField* Field = GMode->GField;
	if (!Field)
	{
		UE_LOG(LogTemp, Error, TEXT("Field null Rook"));
		return;
	}

	XMove   = IsHumanPlayer ? 1 : -1;


	for (int32 k = 0; k < 8; k++)
	{
		if (CheckCoord(x + XMove, y) && !MarkedForward)
		{
			if (Field->CheckSituation)
				CheckMateSituation(x + XMove, y, PlayerNumber, IsHumanPlayer, MarkedForward);
			else
				Mark(x + XMove, y, PlayerNumber, IsHumanPlayer, MarkedForward);
		}
		if (CheckCoord(x - XMove, y) && !MarkedBackwards)
		{
			if (Field->CheckSituation)
				CheckMateSituation(x - XMove, y, PlayerNumber, IsHumanPlayer, MarkedBackwards);
			else
				Mark(x - XMove, y, PlayerNumber, IsHumanPlayer, MarkedBackwards);
		}

		IsHumanPlayer ? XMove++ : XMove--;
	}

	YMove = IsHumanPlayer ? 1 : -1;
	MarkedForward = false;
	MarkedBackwards = false;
	for (int32 k = 0; k < 8; k++)
	{
		if (CheckCoord(x, y + YMove) && !MarkedForward)
		{
			if (Field->CheckSituation)
				CheckMateSituation(x, y + YMove, PlayerNumber, IsHumanPlayer, MarkedForward);
			else
				Mark(x, y + YMove, PlayerNumber, IsHumanPlayer, MarkedForward);
		}

		if (CheckCoord(x, y - YMove) && !MarkedBackwards)
		{
			if (Field->CheckSituation)
				CheckMateSituation(x, y - YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);
			else
				Mark(x, y - YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);
		}

		if (MarkedForward && MarkedBackwards) break;

		IsHumanPlayer ? YMove++ : YMove--;
	}
}

