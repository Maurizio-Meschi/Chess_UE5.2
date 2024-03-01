// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MainActor/ChessPieces.h"
#include "Knight.generated.h"

class AGameField;

UCLASS()
class CHESS_API AKnight : public AChessPieces
{
	GENERATED_BODY()

public:
	AKnight();

	// every piece has a value
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Value;

	// defines the movements of the piece
	void LegalMove(int32 PlayerNumber, bool IsHumanPlayer) override;
};