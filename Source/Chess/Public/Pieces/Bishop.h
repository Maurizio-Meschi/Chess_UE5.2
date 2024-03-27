// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MainActor/ChessPieces.h"
#include "Bishop.generated.h"

struct FBoard;
class AGameField;

UCLASS()
class CHESS_API ABishop : public AChessPieces
{
	GENERATED_BODY()

public:
	ABishop();

	bool LegalMove(FBoard& Board, int32 PlayerNumber, bool CheckFlag) override;
};