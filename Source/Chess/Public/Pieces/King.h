// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MainActor/ChessPieces.h"
#include "King.generated.h"

struct FBoard;
class AGameField;

UCLASS()
class CHESS_API AKing : public AChessPieces
{
	GENERATED_BODY()

public:
	AKing();

	bool NeverMoved = true;

	// defines the movements of the piece
	bool LegalMove(FBoard& Board, int32 PlayerNumber, bool CheckFlag) override;
};