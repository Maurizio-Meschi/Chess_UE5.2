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

	// The pawn can make a different move on its first play
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 First_play;

	// defines the movements of the piece
	void LegalMove();

	// defines what happens when the piece is captured
	void Capture();
};