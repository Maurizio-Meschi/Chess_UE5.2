// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../MainActor/ChessPieces.h"

/**
 * 
 */
class CHESS_API PieceSquareTables
{
public:
	PieceSquareTables();
	~PieceSquareTables();

	static int32 PieceValue(int32 x, int32 y, FString Piece, bool IsMiddleGame, EPieceColor Color);

private:
	
	static TArray<TArray<int32>> MG_PawnTable;

	static TArray<TArray<int32>> MG_KnightTable;

	static TArray<TArray<int32>> MG_BishopTable;

	static TArray<TArray<int32>> MG_RookTable;

	static TArray<TArray<int32>> MG_QueenTable;

	static TArray<TArray<int32>> MG_KingTable;

	static TArray<TArray<int32>> EG_PawnTable;

	static TArray<TArray<int32>> EG_KnightTable;

	static TArray<TArray<int32>> EG_BishopTable;

	static TArray<TArray<int32>> EG_RookTable;

	static TArray<TArray<int32>> EG_QueenTable;

	static TArray<TArray<int32>> EG_KingTable;
};
