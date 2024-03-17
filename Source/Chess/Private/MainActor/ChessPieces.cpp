// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/MainActor/ChessPieces.h"
#include "Pieces/ChessPawn.h"
#include "../Public/MainActor/GameField.h"
#include "../Public/MainActor/Tile.h"
#include "../Public/Manager/Chess_GameMode.h"

// Sets default values
AChessPieces::AChessPieces()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PieceGridPosition = FVector2D(0, 0);

	Color = EPieceColor::WHITE;
}

// Called when the game starts or when spawned

void AChessPieces::BeginPlay()
{
	Super::BeginPlay();
}

bool AChessPieces::CheckCoord(int32 x, int32 y)
{
	if (x < 0 || x > 7 || y < 0 || y > 7) 
		return false;
	return true;
}

void AChessPieces::SetGridPosition(const double InX, const double InY)
{
	PieceGridPosition.Set(InX, InY);
}

void AChessPieces::SetColor(EPieceColor color)
{
	Color = color;
}

void AChessPieces::ResetTileStatus(ATile* CurrTile, ATile* NewTile, int32 PlayerNumber, bool IsTileEmpty)
{
	if (IsTileEmpty)
	{
		CurrTile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);
		NewTile->SetTileStatus(-1, ETileStatus::EMPTY);

	}
	else
	{
		CurrTile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);
		NewTile->SetTileStatus(PlayerNumber == 0 ? 1 : 0, ETileStatus::OCCUPIED);
	}
	NewTile->SetVirtualStatus(EVirtualOccupied::VIRTUAL_EMPTY);
}

void AChessPieces::MarkTile(FBoard& Board, int32 x, int32 y, int32 PlayerNumber, bool& Marked)
{
	AChess_GameMode* GMode = nullptr;
	AGameField* GField = nullptr;
	AManagePiece* ManagerPiece = nullptr;

	if (!FGameRef::GetGameRef(this, GMode, GField, ManagerPiece, "ChessPieces"))
		return;
	
	//auto TileMap = GField->GetTileMap();
	//auto PiecesMap = GField->GetPiecesMap();

	ATile* SelectedTile = nullptr;
	ATile* CurrTile = nullptr;

	if (Board.Field.Contains(FVector2D(x, y)))
		SelectedTile = Board.Field[FVector2D(x, y)];

	if (Board.Field.Contains(this->GetGridPosition()))
		CurrTile = Board.Field[this->GetGridPosition()];

	if (SelectedTile && CurrTile)
	{
		if (SelectedTile->GetTileStatus() == ETileStatus::EMPTY)
		{
			//UE_LOG(LogTemp, Error, TEXT("Tile x=%d y=%d is empty"), x, y);
			if (this->IsA<AChessPawn>() && Cast<AChessPawn>(this)->CaptureSituation)
				return;

			// Metto l'attuale tile vuota e quella selezionata occupata
			SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);
			SelectedTile->SetVirtualStatus(this->IsA<AKing>() ? EVirtualOccupied::VIRTUAL_OCCUPIED_BY_KING : EVirtualOccupied::VIRTUAL_OCCUPIED);
			CurrTile->SetTileStatus(-1, ETileStatus::EMPTY);

			// Vedo se la mossa è legale. Se vero ripristino gli stati corretti delle tile, altrimenti ed esco, altrimenti lo alloco
			auto Pieces = (PlayerNumber == 1 ? GField->GetHumanPlayerPieces() : GField->GetBotPieces());
			for (auto EnemyPiece : Pieces)
			{
				if (!Board.CapturePieces.Contains(EnemyPiece))
				{
					if (EnemyPiece->LegalMove(Board, PlayerNumber == 0 ? 1 : 0, true))
					{
						ResetTileStatus(CurrTile, SelectedTile, PlayerNumber, true);
						return;
					}
				}
			}
			ResetTileStatus(CurrTile, SelectedTile, PlayerNumber, true);
			FMarked Obj;
			Obj.Tile = SelectedTile;
			Obj.Capture = false;
			ManagerPiece->TileMarkedForPiece[this->IndexArray].Add(Obj);

			// Arrocco
			if (IsA<AKing>() && x == 0 && y == 5 && Cast<AKing>(this)->NeverMoved)
			{
				Castling(Board, FVector2D(0, 6), FVector2D(0, 7), PlayerNumber, Marked);
			}

			// Arrocco Lungo
			if (IsA<AKing>() && x == 0 && y == 3 && Cast<AKing>(this)->NeverMoved)
			{
				if (Board.Field.Contains(FVector2D(0, 1)))
					if (Board.Field[FVector2D(0, 1)]->GetTileStatus() != ETileStatus::EMPTY)
						return;

				Castling(Board, FVector2D(0, 2), FVector2D(0, 0), PlayerNumber, Marked);
			}

			// Arrocco nemico
			if (IsA<AKing>() && x == 7 && y == 5 && Cast<AKing>(this)->NeverMoved)
			{
				Castling(Board, FVector2D(7, 6), FVector2D(7, 7), PlayerNumber, Marked);
			}

			// Arrocco Lungo nemico
			if (IsA<AKing>() && x == 7 && y == 3 && Cast<AKing>(this)->NeverMoved)
			{
				if (Board.Field.Contains(FVector2D(7, 1)))
					if (Board.Field[FVector2D(7, 1)]->GetTileStatus() != ETileStatus::EMPTY)
						return;

				Castling(Board, FVector2D(7, 2), FVector2D(7, 0), PlayerNumber, Marked);
			}
		}
		else if (SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
		{
			if (this->IsA<AChessPawn>() && !Cast<AChessPawn>(this)->CaptureSituation)
			{
				Marked = true;
				return;
			}

			if (Board.Pieces.Contains(SelectedTile->GetGridPosition()))
			{
				auto Piece = Board.Pieces[SelectedTile->GetGridPosition()];
				if (this->Color == Piece->Color)
				{
					Marked = true;
					return;
				}
			}

			if (SelectedTile->GetOwner() != PlayerNumber)
			{
				SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);
				SelectedTile->SetVirtualStatus(this->IsA<AKing>() ? EVirtualOccupied::VIRTUAL_OCCUPIED_BY_KING : EVirtualOccupied::VIRTUAL_OCCUPIED);
				CurrTile->SetTileStatus(-1, ETileStatus::EMPTY);

				auto Pieces = (PlayerNumber == 1 ? GField->GetHumanPlayerPieces() : GField->GetBotPieces());
				for (auto EnemyPiece : Pieces)
				{
					if (!Board.CapturePieces.Contains(EnemyPiece))
					{
						if (EnemyPiece->LegalMove(Board, PlayerNumber == 0 ? 1 : 0, true))
						{
							ResetTileStatus(CurrTile, SelectedTile, PlayerNumber, false);
							Marked = true;
							return;
						}
					}
				}
				ResetTileStatus(CurrTile, SelectedTile, PlayerNumber, false);
				FMarked Obj;
				Obj.Tile = SelectedTile;
				Obj.Capture = true;
				ManagerPiece->TileMarkedForPiece[this->IndexArray].Add(Obj);
			}
			Marked = true;
		}
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Error in select tile in ChessPieces"));

}


void AChessPieces::Castling(FBoard& Board, FVector2D TilePosition, FVector2D RookPosition, int32 PlayerNumber, bool& Marked)
{
	AChess_GameMode* GMode = nullptr;
	AGameField* GField = nullptr;
	AManagePiece* ManagerPiece = nullptr;

	if (!FGameRef::GetGameRef(this, GMode, GField, ManagerPiece, "ChessPieces"))
		return;

	auto TileMap = GField->GetTileMap();
	auto PiecesMap = GField->GetPiecesMap();

	ATile* NextTile = nullptr;
	if (TileMap.Contains(TilePosition))
		NextTile = TileMap[TilePosition];
	if (NextTile && NextTile->GetTileStatus() == ETileStatus::EMPTY)
	{
		AChessPieces* Rook = nullptr;
		if (PiecesMap.Contains(RookPosition))
		{
			Rook = PiecesMap[RookPosition];
			if (Rook->IsA<ARook>() && Cast<ARook>(Rook)->NeverMoved)
			{
				MarkTile(Board, TilePosition.X, TilePosition.Y, PlayerNumber, Marked);
				auto TileMarked = ManagerPiece->TileMarkedForPiece[this->IndexArray];
				for (auto Element : TileMarked)
				{
					if (Element.Tile->GetGridPosition() == TilePosition)
						AManagePiece::Castling = true;
				}
			}
		}
	}
}


bool AChessPieces::TestCheck(FBoard& Board, int32 x, int32 y, int32 PlayerNumber, bool& Marked)
{
	AGameField* GField = nullptr;

	if (!FGameRef::GetGameField(this, GField, "ChessPieces"))
		return false;

	ATile* SelectedTile = nullptr;

	// Se il pezzo è in una tile VIRTUAl_OCCUPIED è come se fosse stata mangiata e non devo considerarla
	if (Board.Field.Contains(this->GetGridPosition()))
		SelectedTile = Board.Field[this->GetGridPosition()];

	if (SelectedTile && SelectedTile->GetVirtaulStatus() == EVirtualOccupied::VIRTUAL_OCCUPIED)
	{
		Marked = true;
		return false;
	}

	SelectedTile = nullptr;

	if (Board.Field.Contains(FVector2D(x, y)))
		SelectedTile = Board.Field[FVector2D(x, y)];

	if (SelectedTile)
	{
		if (SelectedTile->GetTileStatus() == ETileStatus::EMPTY)
		{
			return false;
		}
		else if (SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
		{
			if (this->IsA<AChessPawn>() && !Cast<AChessPawn>(this)->CaptureSituation)
				return false;

			if (SelectedTile->GetVirtaulStatus() == EVirtualOccupied::VIRTUAL_OCCUPIED_BY_KING)
			{
				return true;
			}
			else if (SelectedTile->GetVirtaulStatus() == EVirtualOccupied::VIRTUAL_OCCUPIED)
			{
				Marked = true;
				return false;
			}
			else
			{
				AChessPieces* Piece = nullptr;
				if (Board.Pieces.Contains(SelectedTile->GetGridPosition()))
					Piece = Board.Pieces[SelectedTile->GetGridPosition()];

				if (!Piece)
				{
					UE_LOG(LogTemp, Error, TEXT("Piece null in CheckTest"));
					return false;
				}

				if (Piece->IsA<AKing>() && SelectedTile->GetOwner() != PlayerNumber) //Piece->Color == (PlayerNumber == 0 ? EPieceColor::BLACK : EPieceColor::WHITE))
				{
					return true;
				}
				else
					Marked = true;
			}
		}
		return false;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Selected tile null in ChessPieces"));
		return false;
	}
}