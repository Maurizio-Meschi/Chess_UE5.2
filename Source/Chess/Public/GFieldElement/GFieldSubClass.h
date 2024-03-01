// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../MainActor/Tile.h"
#include "../Pieces/Bishop.h"
#include "../Pieces/ChessPawn.h"
#include "../Pieces/King.h"
#include "../Pieces/Knight.h"
#include "../Pieces/Queen.h"
#include "../Pieces/Rook.h"
#include "../MainActor/ChessPieces.h"
#include "CoreMinimal.h"
#include "GFieldSubClass.generated.h"

USTRUCT()
struct CHESS_API FGFieldTSubClass
{
	GENERATED_BODY()

	FGFieldTSubClass();

	FGFieldTSubClass& operator=(const FGFieldTSubClass& Other);
	
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<ATile>> TileClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATile> TileClassMarked;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATile> TileClassPieceToCapture;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<ARook>> ChessRook;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AKing>> ChessKing;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AKnight>> ChessKnight;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AQueen>> ChessQueen;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<ABishop>> ChessBishop;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AChessPawn>> ChessPawn;
};