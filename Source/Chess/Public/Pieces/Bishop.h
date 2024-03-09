// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MainActor/ChessPieces.h"
#include "Bishop.generated.h"

class AGameField;

UCLASS()
class CHESS_API ABishop : public AChessPieces
{
	GENERATED_BODY()

public:
	ABishop();

	// every piece has a value
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Value;

	// defines the movements of the piece
	bool LegalMove(int32 PlayerNumber, bool CheckFlag) override;
};