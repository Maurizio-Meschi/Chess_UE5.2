// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Public/MainActor/ChessPieces.h"
#include "TFR.generated.h"


/**
 * 
 */
UCLASS()
class CHESS_API UTFR : public UObject
{
	GENERATED_BODY()

public:
	UTFR() = default;
	bool IsDraw();

private:
	TArray<FString> FEN_Array;

	FString Tokenizer(AChessPieces* Piece);
	FString GenerateString();
	
};


