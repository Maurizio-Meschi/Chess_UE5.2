// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MainActor/ChessPieces.h"
#include "Rook.generated.h"

struct FBoard;
class AGameField;

UCLASS()
class CHESS_API ARook : public AChessPieces
{
	GENERATED_BODY()
public:
	ARook();

	bool NeverMoved = true;

	bool LegalMove(FBoard& Board, int32 PlayerNumber, bool CheckFlag) override;
};