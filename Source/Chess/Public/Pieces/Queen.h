// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../ChessPieces.h"
#include "Queen.generated.h"

class AGameField;

UCLASS()
class CHESS_API AQueen : public AChessPieces
{
	GENERATED_BODY()

public:
	AQueen();

	// every piece has a value
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Value;

	// defines the movements of the piece
	void LegalMove() override;

	// defines what happens when the piece is captured
	void Capture();
};