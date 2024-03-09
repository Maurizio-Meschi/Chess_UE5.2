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

void AChessPieces::MarkTile(int32 x, int32 y, int32 PlayerNumber, bool& Marked)
{
	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null in ManagePiece"));
		return;
	}

	auto GField = GMode->GField;
	if (!GField)
	{
		UE_LOG(LogTemp, Error, TEXT("Field null in ManagePiece"));
		return;
	}

	auto ManagerPiece = GMode->Manager;
	if (!ManagerPiece)
	{
		UE_LOG(LogTemp, Error, TEXT("Manager Piece null in ChessPieces"));
		return;
	}
	auto TileMap = GField->GetTileMap();
	auto PiecesMap = GField->GetPiecesMap();

	ATile* SelectedTile = nullptr;
	ATile* CurrTile = nullptr;

	if (TileMap.Contains(FVector2D(x, y)))
		SelectedTile = TileMap[FVector2D(x, y)];

	if (TileMap.Contains(this->GetGridPosition()))
		CurrTile = TileMap[this->GetGridPosition()];

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
				if (EnemyPiece->LegalMove(PlayerNumber == 0? 1:0, true))
				{
					ResetTileStatus(CurrTile, SelectedTile, PlayerNumber, true);
					return;
				}
			}
			ResetTileStatus(CurrTile, SelectedTile, PlayerNumber, true);
			FMarked Obj;
			Obj.Tile = SelectedTile;
			Obj.Capture = false;
			ManagerPiece->TileMarkedForPiece[this->IndexArray].Add(Obj);
		}
		else if (SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
		{
			if (this->IsA<AChessPawn>() && !Cast<AChessPawn>(this)->CaptureSituation)
				return;

			if (SelectedTile->GetOwner() != PlayerNumber)
			{
				SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);
				SelectedTile->SetVirtualStatus(this->IsA<AKing>() ? EVirtualOccupied::VIRTUAL_OCCUPIED_BY_KING : EVirtualOccupied::VIRTUAL_OCCUPIED);
				CurrTile->SetTileStatus(-1, ETileStatus::EMPTY);

				auto Pieces = (PlayerNumber == 1 ? GField->GetHumanPlayerPieces() : GField->GetBotPieces());
				for (auto EnemyPiece : Pieces)
				{
					if (EnemyPiece->LegalMove(PlayerNumber == 0? 1:0, true))
					{
						ResetTileStatus(CurrTile, SelectedTile, PlayerNumber, false);
						return;
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

bool AChessPieces::TestCheck(int32 x, int32 y, int32 PlayerNumber, bool& Marked)
{
	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null in ManagePiece"));
		return false;
	}

	auto GField = GMode->GField;
	if (!GField)
	{
		UE_LOG(LogTemp, Error, TEXT("Field null in ManagePiece"));
		return false;
	}

	auto ManagerPiece = GMode->Manager;
	if (!ManagerPiece)
	{
		UE_LOG(LogTemp, Error, TEXT("Manager Piece null in ChessPieces"));
		return false;
	}
	auto TileMap = GField->GetTileMap();
	auto PiecesMap = GField->GetPiecesMap();


	ATile* SelectedTile = nullptr;

	// Se il pezzo è in una tile VIRTUAl_OCCUPIED è come se fosse stata mangiata e non devo considerarla
	if (TileMap.Contains(this->GetGridPosition()))
		SelectedTile = TileMap[this->GetGridPosition()];

	if (SelectedTile && SelectedTile->GetVirtaulStatus() == EVirtualOccupied::VIRTUAL_OCCUPIED)
	{
		Marked = true;
		return false;
	}

	SelectedTile = nullptr;

	if (TileMap.Contains(FVector2D(x, y)))
		SelectedTile = TileMap[FVector2D(x, y)];

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
				if (PiecesMap.Contains(SelectedTile->GetGridPosition()))
					Piece = PiecesMap[SelectedTile->GetGridPosition()];

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
