// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../ChessPieces.h"
#include "King.generated.h"

class AGameField;

UCLASS()
class CHESS_API AKing : public AChessPieces
{
	GENERATED_BODY()

public:
	AKing();

	// defines the movements of the piece
	void LegalMove(int32 PlayerNumber, bool IsHumanPlayer) override;

	// defines what happens when the piece is captured
	void Capture();
};