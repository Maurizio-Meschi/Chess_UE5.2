// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Pieces/Queen.h"
#include "../Public/Manager/Chess_GameMode.h"

AQueen::AQueen()
{
	Value = 9;

	Name = "Q";
}

void AQueen::LegalMove(int32 PlayerNumber, bool IsHumanPlayer)
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
		UE_LOG(LogTemp, Error, TEXT("Game mode null Queen"));
		return;
	}

	AGameField* Field = GMode->GField;
	if (!Field)
	{
		UE_LOG(LogTemp, Error, TEXT("Field null Queen"));
		return;
	}

	XMove = IsHumanPlayer ? 1 : -1;
	YMove = IsHumanPlayer ? 1 : -1;

	if (Field->Direction == "Positive Oblique" || Field->Direction == "None")
	{
		for (int32 k = 0; k < 8; k++)
		{
			if (CheckCoord(x + XMove, y + YMove) && !MarkedForward)
			{
				if (Field->CheckLegalMove)
					CheckIfAllMoveIsLegal(x + XMove, y + YMove, PlayerNumber, IsHumanPlayer, MarkedForward);
				else if (Field->CheckSituation)
					CheckMateSituation(x + XMove, y + YMove, PlayerNumber, IsHumanPlayer, MarkedForward);
				else
					Mark(x + XMove, y + YMove, PlayerNumber, IsHumanPlayer, MarkedForward);
			}

			if (CheckCoord(x - XMove, y - YMove) && !MarkedBackwards)
			{
				if (Field->CheckLegalMove)
					CheckIfAllMoveIsLegal(x - XMove, y - YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);
				else if (Field->CheckSituation)
					CheckMateSituation(x - XMove, y - YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);
				else
					Mark(x - XMove, y - YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);
			}

			IsHumanPlayer ? XMove++ : XMove--;
			IsHumanPlayer ? YMove++ : YMove--;
		}
	}
	Field->Support.Empty();

	XMove = IsHumanPlayer ? 1 : -1;
	YMove = IsHumanPlayer ? 1 : -1;
	MarkedForward = false;
	MarkedBackwards = false;

	if (Field->Direction == "Negative Oblique" || Field->Direction == "None")
	{
		for (int32 k = 0; k < 8; k++)
		{
			if (CheckCoord(x + XMove, y - YMove) && !MarkedForward)
			{
				if (Field->CheckLegalMove)
					CheckIfAllMoveIsLegal(x + XMove, y - YMove, PlayerNumber, IsHumanPlayer, MarkedForward);
				else if (Field->CheckSituation)
					CheckMateSituation(x + XMove, y - YMove, PlayerNumber, IsHumanPlayer, MarkedForward);
				else
					Mark(x + XMove, y - YMove, PlayerNumber, IsHumanPlayer, MarkedForward);
			}

			if (CheckCoord(x - XMove, y + YMove) && !MarkedBackwards)
			{
				if (Field->CheckLegalMove)
					CheckIfAllMoveIsLegal(x - XMove, y + YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);
				else if (Field->CheckSituation)
					CheckMateSituation(x - XMove, y + YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);
				else
					Mark(x - XMove, y + YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);
			}

			IsHumanPlayer ? XMove++ : XMove--;
			IsHumanPlayer ? YMove++ : YMove--;
		}
	}
	Field->Support.Empty();


	XMove = IsHumanPlayer ? 1 : -1;
	MarkedForward = false;
	MarkedBackwards = false;

	if (Field->Direction == "Vertical" || Field->Direction == "None")
	{
		for (int32 k = 0; k < 8; k++)
		{
			if (CheckCoord(x + XMove, y) && !MarkedForward)
			{
				if (Field->CheckLegalMove)
					CheckIfAllMoveIsLegal(x + XMove, y, PlayerNumber, IsHumanPlayer, MarkedForward);
				else if (Field->CheckSituation)
					CheckMateSituation(x + XMove, y, PlayerNumber, IsHumanPlayer, MarkedForward);
				else
					Mark(x + XMove, y, PlayerNumber, IsHumanPlayer, MarkedForward);
			}

			if (CheckCoord(x - XMove, y) && !MarkedBackwards)
			{
				if (Field->CheckLegalMove)
					CheckIfAllMoveIsLegal(x - XMove, y, PlayerNumber, IsHumanPlayer, MarkedBackwards);
				else if (Field->CheckSituation)
					CheckMateSituation(x - XMove, y, PlayerNumber, IsHumanPlayer, MarkedBackwards);
				else
					Mark(x - XMove, y, PlayerNumber, IsHumanPlayer, MarkedBackwards);
			}

			IsHumanPlayer ? XMove++ : XMove--;
		}
	}
	Field->Support.Empty();

	YMove = IsHumanPlayer ? 1 : -1;
	MarkedForward = false;
	MarkedBackwards = false;

	if (Field->Direction == "Horizontal" || Field->Direction == "None")
	{
		for (int32 k = 0; k < 8; k++)
		{
			if (CheckCoord(x, y + YMove) && !MarkedForward)
			{
				if (Field->CheckLegalMove)
					CheckIfAllMoveIsLegal(x, y + YMove, PlayerNumber, IsHumanPlayer, MarkedForward);
				else if (Field->CheckSituation)
					CheckMateSituation(x, y + YMove, PlayerNumber, IsHumanPlayer, MarkedForward);
				else
					Mark(x, y + YMove, PlayerNumber, IsHumanPlayer, MarkedForward);
			}

			if (CheckCoord(x, y - YMove) && !MarkedBackwards)
			{
				if (Field->CheckLegalMove)
					CheckIfAllMoveIsLegal(x, y - YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);
				else if (Field->CheckSituation)
					CheckMateSituation(x, y - YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);
				else
					Mark(x, y - YMove, PlayerNumber, IsHumanPlayer, MarkedBackwards);
			}

			if (MarkedForward && MarkedBackwards) break;

			IsHumanPlayer ? YMove++ : YMove--;
		}
	}
	Field->Support.Empty();
}