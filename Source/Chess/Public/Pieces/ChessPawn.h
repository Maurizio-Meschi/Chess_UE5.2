// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MainActor/ChessPieces.h"
#include "ChessPawn.generated.h"

struct FBoard;
class AGameField;

UCLASS()
class CHESS_API AChessPawn : public AChessPieces
{
	GENERATED_BODY()

public:
	AChessPawn();

	bool CaptureSituation;

	// defines the movements of the piece
	bool LegalMove(FBoard& Board, int32 PlayerNumber, bool CheckFlag) override;
};