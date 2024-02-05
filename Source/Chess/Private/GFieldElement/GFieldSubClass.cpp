// Fill out your copyright notice in the Description page of Project Settings.

#include "../../Public/GFieldElement/GFieldSubClass.h"


FGFieldTSubClass::FGFieldTSubClass()
{
	TileClass.SetNum(2);
	ChessRook.SetNum(2);
	ChessKing.SetNum(2);
	ChessKnight.SetNum(2);
	ChessQueen.SetNum(2);
	ChessBishop.SetNum(2);
	ChessPawn.SetNum(2);
}

FGFieldTSubClass& FGFieldTSubClass::operator=(const FGFieldTSubClass& Other)
{
	if (this != &Other)
	{
		// Copia gli array di oggetti TSubclassOf
		TileClass = Other.TileClass;
		TileClassMarked = Other.TileClassMarked;
		TileClassPieceToCapture = Other.TileClassPieceToCapture;
		ChessRook = Other.ChessRook;
		ChessKing = Other.ChessKing;
		ChessKnight = Other.ChessKnight;
		ChessQueen = Other.ChessQueen;
		ChessBishop = Other.ChessBishop;
		ChessPawn = Other.ChessPawn;
	}
	return *this;
}