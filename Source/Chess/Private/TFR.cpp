// Fill out your copyright notice in the Description page of Project Settings.

#include "TFR.h"
#include "../Public/Manager/ManagePiece.h"
#include "../Public/Manager/Chess_GameMode.h"
#include "../Public/MainActor/ChessPieces.h"
#include "../Public/MainActor/GameField.h"


FString UTFR::Tokenizer(AChessPieces* Piece)
{
	if (Piece->IsA<ABishop>())
	{
		if (Piece->Color == EPieceColor::BLACK)
			return "b";
		else
			return "B";
	}
	else if (Piece->IsA<AChessPawn>())
	{
		if (Piece->Color == EPieceColor::BLACK)
			return "p";
		else
			return "P";
	}
	else if (Piece->IsA<AKing>())
	{
		if (Piece->Color == EPieceColor::BLACK)
			return "k";
		else
			return "K";
	}
	else if (Piece->IsA<AKnight>())
	{
		if (Piece->Color == EPieceColor::BLACK)
			return "n";
		else
			return "N";
	}
	else if (Piece->IsA<AQueen>())
	{
		if (Piece->Color == EPieceColor::BLACK)
			return "q";
		else
			return "Q";
	}
	else if (Piece->IsA<ARook>())
	{
		if (Piece->Color == EPieceColor::BLACK)
			return "r";
		else
			return "R";
	}
	else
		return "";
}

FString UTFR::GenerateString()
{
	AChess_GameMode* GMode = nullptr;
	AGameField* GField = nullptr;
	AManagePiece* PieceManager = nullptr;
	if (!FGameRef::GetGameRef(this, GMode, GField, PieceManager, "Minimax"))
		return "";

	FString tmp;
	int32 x = 0;
	int32 Cont = 0;
	int32  Normalization = 1;

	auto FieldMap = GField->GetTileMap();
	auto PiecesMap = GField->GetPiecesMap();

	for (auto Element : FieldMap)
	{
		ATile* Tile = Element.Value;
		auto TilePosition = Tile->GetGridPosition();

		if (PiecesMap.Contains(TilePosition))
		{
			AChessPieces* Piece = PiecesMap[TilePosition];
			if (Cont != 0)
				tmp += (FString::FromInt(Cont) + Tokenizer(Piece));
			else 
				tmp += Tokenizer(Piece);
			Cont = 0;
		}
		else
			++Cont;

		x++;

		if (Cont == 8)
		{
			tmp += "8";
			Cont = 0;
		}

		if (x == Normalization * 8)
		{
			if (Cont != 0)
			{
				tmp += (FString::FromInt(Cont) + "/");
				Cont = 0;
			}
			else
				tmp += "/";

			Normalization++;
		}
	}
	return tmp;
}

bool UTFR::IsDraw()
{
	FString tmp = GenerateString();

	//UE_LOG(LogTemp, Error, TEXT("%s"), *tmp);
	
	FEN_Array.Add(tmp);
	UE_LOG(LogTemp, Error, TEXT("%d"), FEN_Array.Num());
	
	int Occurrences = 0;

	// Attraversa l'array di stringhe e controlla ogni elemento
	for (auto String : FEN_Array)
	{
		//UE_LOG(LogTemp, Error, TEXT("%d"), FEN_Array.Num());
		if (String == tmp)
		{
			Occurrences++;
		}
	}
	//UE_LOG(LogTemp, Error, TEXT("\n"));
	//UE_LOG(LogTemp, Error, TEXT("%d"), Occurrences);
	return Occurrences >= 3;
}
