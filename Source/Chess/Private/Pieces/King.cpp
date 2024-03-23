// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Pieces/King.h"
#include "../Public/Manager/Chess_GameMode.h"

AKing::AKing()
{
	Name = "K";

	Value = 0;
}

bool AKing::LegalMove(FBoard& Board, int32 PlayerNumber, bool CheckFlag)
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

	for (int32 i = 0; i < 3; ++i)
	{
		for (int32 j = 0; j < 3; ++j)
		{
			if (i == 1 && j == 1)
				continue;
			XMove = i - 1;
			YMove = j - 1;

			


			if (CheckCoord(x + XMove, y + YMove)) //&& !MarkedForward)
			{
				if (!CheckFlag)
				{
					if (Color == EPieceColor::BLACK && !CheckFlag)
						UE_LOG(LogTemp, Error, TEXT("Controllo x=%d y=%d"), x + XMove, y + YMove);
					MarkTile(Board, x + XMove, y + YMove, PlayerNumber, MarkedForward);
				}
				else
					if (TestCheck(Board, x + XMove, y + YMove, PlayerNumber, MarkedForward))
						return true;
			}
		}
	}
	return false;
}