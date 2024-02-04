// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../ChessPieces.h"
#include "ChessPawn.generated.h"

class AGameField;

UCLASS()
class CHESS_API AChessPawn : public AChessPieces
{
	GENERATED_BODY()

public:
	AChessPawn();

	// every piece has a value
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Value;

	// defines the movements of the piece
	void LegalMove(int32 PlayerNumber, bool IsHumanPlayer) override;

	void Mark(int32 x, int32 y, int32 PlayerNumber);
	void MarkToCapture(int32 x, int32 y, int32 PlayerNumber, bool IsHumanPlayer);
};