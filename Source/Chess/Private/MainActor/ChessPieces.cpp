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

	IsKing = false;
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

void AChessPieces::Mark(int32 x, int32 y, int32 PlayerNumber, bool IsHumanPlayer, bool& Marked)
{

	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null in ChessPieces"));
		return;
	}

	AGameField* Field = GMode->GField;
	if (!Field)
	{ 
		UE_LOG(LogTemp, Error, TEXT("Game field null in ChessPieces"));
		return;
	}

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

	ATile* SelectedTile = nullptr;

	
	if (TileMap.Contains(FVector2D(x, y)))
		SelectedTile = TileMap[FVector2D(x, y)];
	

	if (!SelectedTile)
	{
		UE_LOG(LogTemp, Error, TEXT("SelectedTile null in ChessPieces"));
		return;
	}

	if (this->IsA<AKing>() &&
		SelectedTile->GetStatusCheckmate() == EStatusCheckmate::BLOCK_KING)
	{
		return;
	}
	else if(SelectedTile->GetTileStatus() == ETileStatus::EMPTY)
	{
		SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED);
		Field->AddTileMarked(SelectedTile);
	}
	else if (SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
	{
		AChessPieces* SelectedPiece = nullptr;
		
		if (PiecesMap.Contains(FVector2D(x, y)))
			SelectedPiece = PiecesMap[FVector2D(x, y)];
		

		if (SelectedPiece && SelectedPiece->Color == (IsHumanPlayer ? EPieceColor::BLACK : EPieceColor::WHITE))
		{
			if (!SelectedPiece->IsA<AKing>())
			{
				SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED_TO_CAPTURE);
				Field->AddTileMarked(SelectedTile);
			}
		}
		Marked = true;
	}
}

void AChessPieces::CheckMateSituation(int32 x, int32 y, int32 PlayerNumber, bool IsHumanPlayer, bool& Marked)
{	
	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null in ChessPieces"));
		return;
	}

	AGameField* Field = GMode->GField;
	if (!Field)
	{
		UE_LOG(LogTemp, Error, TEXT("Game field null in ChessPieces"));
		return;
	}

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

	ATile* SelectedTile = nullptr;

	
	if (TileMap.Contains(FVector2D(x, y)))
		SelectedTile = TileMap[FVector2D(x, y)];
	

	if (!SelectedTile)
	{
		UE_LOG(LogTemp, Error, TEXT("SelectedTile null in ChessPieces"));
		return;
	}

	if (Field->IsCheckmateSituation)
	{
		ManageCheckMateSituation(PlayerNumber, IsHumanPlayer, Marked, SelectedTile);
	}
	else
	{
		// Questa è valida solo per il king
		if (IsKing)
			ManageCheckSituationKing(x, y, PlayerNumber, IsHumanPlayer, SelectedTile);

		// controllo se una pedina può andare diretta contro il re
		else if (SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
		{
			ManageCheckSituationOccpied(x, y, PlayerNumber, IsHumanPlayer, SelectedTile, Marked);
		}
		// controllo se una pedina può andare in una cella in cui si potrebbe spostare il re
		if (!IsKing && (SelectedTile->GetStatusCheckmate() == EStatusCheckmate::MARK_BY_KING || 
			SelectedTile->GetStatusCheckmate() == EStatusCheckmate::CAPTURE_BY_KING || 
			SelectedTile->GetStatusCheckmate() == EStatusCheckmate::CAPTURE_TO_AVOID_CHECKMATE))
		{
			SelectedTile->SetStatusCheckmate(PlayerNumber, EStatusCheckmate::BLOCK_KING);
		}
	}
}

void AChessPieces::ManageCheckMateSituation(int32 PlayerNumber, bool IsHumanPlayer, bool& Marked, ATile* SelectedTile)
{
	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null in ChessPieces"));
		return;
	}

	AGameField* Field = GMode->GField;
	if (!Field)
	{
		UE_LOG(LogTemp, Error, TEXT("Game field null in ChessPieces"));
		return;
	}

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

	if (!SelectedTile)
	{
		UE_LOG(LogTemp, Error, TEXT("SelectedTile null in ChessPieces"));
		return;
	}

	if (SelectedTile->GetStatusCheckmate() == EStatusCheckmate::MARK_TO_AVOID_CHECKMATE)
	{
		SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED);
		Field->AddTileMarked(SelectedTile);
	}

	if (SelectedTile->GetStatusCheckmate() == EStatusCheckmate::MARK_AND_BLOCK_KING &&
		!this->IsA<AKing>())
	{
		SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED);
		Field->AddTileMarked(SelectedTile);
	}

	// impedisco alle pedine di saltare i pezzi illegalmente
	if (SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
	{
		Marked = true;
	}

	if (SelectedTile->GetStatusCheckmate() == EStatusCheckmate::CAPTURE_BY_KING ||
		SelectedTile->GetStatusCheckmate() == EStatusCheckmate::CAPTURE_TO_AVOID_CHECKMATE)
	{
		SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED_TO_CAPTURE);
		Field->AddTileMarked(SelectedTile);
		Marked = true;
	}

	if (this->IsA<AKing>() && SelectedTile->GetStatusCheckmate() == EStatusCheckmate::MARK_BY_KING)
	{
		SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED);
		Field->AddTileMarked(SelectedTile);
	}
}

void AChessPieces::ManageCheckSituationKing(int32 x, int32 y, int32 PlayerNumber, bool IsHumanPlayer, ATile* SelectedTile)
{
	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null in ChessPieces"));
		return;
	}

	AGameField* Field = GMode->GField;
	if (!Field)
	{
		UE_LOG(LogTemp, Error, TEXT("Game field null in ChessPieces"));
		return;
	}

	if (!SelectedTile)
	{
		UE_LOG(LogTemp, Error, TEXT("SelectedTile null in ChessPieces"));
		return;
	}

	TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

	if (SelectedTile->GetTileStatus() == ETileStatus::EMPTY)
	{
		SelectedTile->SetStatusCheckmate(PlayerNumber, EStatusCheckmate::MARK_BY_KING);
	}
	// Evito che catturando una pedina mi vada ad esporre
	if (SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
	{
		AChessPieces* SelectedPiece = nullptr;

		if (PiecesMap.Contains(FVector2D(x, y)))
			SelectedPiece = PiecesMap[FVector2D(x, y)];
		
		if (SelectedPiece && SelectedPiece->Color == (IsHumanPlayer ? EPieceColor::BLACK : EPieceColor::WHITE))
		{
			if (!SelectedPiece->IsA<AKing>())
			{
				SelectedTile->SetStatusCheckmate(PlayerNumber, EStatusCheckmate::MARK_BY_KING);
			}
		}
	}
}

void AChessPieces::ManageCheckSituationOccpied(int32 x, int32 y, int32 PlayerNumber, bool IsHumanPlayer, ATile* SelectedTile, bool& Marked)
{
	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null in ChessPieces"));
		return;
	}

	AGameField* Field = GMode->GField;
	if (!Field)
	{
		UE_LOG(LogTemp, Error, TEXT("Game field null in ChessPieces"));
		return;
	}

	if (!SelectedTile)
	{
		UE_LOG(LogTemp, Error, TEXT("SelectedTile null in ChessPieces"));
		return;
	}

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

	AChessPieces* SelectedPiece = nullptr;

	
	if (PiecesMap.Contains(FVector2D(x, y)))
		SelectedPiece = PiecesMap[FVector2D(x, y)];
	

	if (SelectedPiece && SelectedPiece->Color == (IsHumanPlayer ? EPieceColor::BLACK : EPieceColor::WHITE) &&
		SelectedPiece->IsA<AKing>())
	{
		Field->KingUnderAttack = true;
		FVector2d PiecePosition = this->GetGridPosition();

		
		if (TileMap.Contains(PiecePosition))
		{
			//if (TileMap[PiecePosition]->GetStatusCheckmate() != EStatusCheckmate::BLOCK_KING)
				TileMap[PiecePosition]->SetStatusCheckmate(PlayerNumber, EStatusCheckmate::CAPTURE_TO_AVOID_CHECKMATE);
		}
		

		// non devo marcare le tile intermedie perchè il cavallo va diretto dal re
		if (!this->IsA<AKnight>())
			FindTileBetweenP1P2(PiecePosition, FVector2D(x, y), PlayerNumber);
	}
	else
		Marked = true;
}

void AChessPieces::FindTileBetweenP1P2(const FVector2D& P1, const FVector2D& P2, int32 PlayerNumber)
{
	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null in ChessPieces"));
		return;
	}

	AGameField* Field = GMode->GField;
	if (!Field)
	{
		UE_LOG(LogTemp, Error, TEXT("Game field null in ChessPieces"));
		return;
	}

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

	ATile* SelectedTile = nullptr;

	int32 deltaX = P2.X - P1.X;
	int32 deltaY = P2.Y - P1.Y;
	int32 stepX = (deltaX > 0) ? 1 : -1;
	int32 stepY = (deltaY > 0) ? 1 : -1;

	int32 x = P1.X;
	int32 y = P1.Y;


	// Bresenham algorithm.
	while (x != P2.X || y != P2.Y)
	{
		if (CheckCoord(x, y) && FVector2D(x, y) != P1 && FVector2D(x, y) != P1) {
			
			UE_LOG(LogTemp, Error, TEXT("x: %d, y: %d"), x, y);
			if (TileMap.Contains(FVector2D(x, y)))
				SelectedTile = TileMap[FVector2D(x, y)];

			if (SelectedTile)
			{
				if (SelectedTile->GetStatusCheckmate() == EStatusCheckmate::MARK_BY_KING || 
					SelectedTile->GetStatusCheckmate() == EStatusCheckmate::BLOCK_KING)
				{
					SelectedTile->SetStatusCheckmate(PlayerNumber, EStatusCheckmate::MARK_AND_BLOCK_KING);
				}
				else
					SelectedTile->SetStatusCheckmate(PlayerNumber, EStatusCheckmate::MARK_TO_AVOID_CHECKMATE);
			}

		}
		int32 offsetX = FMath::Abs(x - P1.X);
		int32 offsetY = FMath::Abs(y - P1.Y);

		if (offsetX < FMath::Abs(deltaX))
		{
			x += stepX;
		}
		if (offsetY < FMath::Abs(deltaY))
		{
			y += stepY;
		}
	}
}

void AChessPieces::CheckIfAllMoveIsLegal(int32 x, int32 y, int32 PlayerNumber, bool IsHumanPlayer, bool& Marked)
{
	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null in ChessPieces"));
		return;
	}

	AGameField* Field = GMode->GField;
	if (!Field)
	{
		UE_LOG(LogTemp, Error, TEXT("Game field null in ChessPieces"));
		return;
	}

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

	ATile* SelectedTile = nullptr;

	if (TileMap.Contains(FVector2D(x, y)))
		SelectedTile = TileMap[FVector2D(x, y)];

	if (SelectedTile->GetTileStatus() == ETileStatus::EMPTY)
	{
		return;
	}
	else if (SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
	{
		AChessPieces* SelectedPiece = nullptr;

		if (PiecesMap.Contains(FVector2D(x, y)))
			SelectedPiece = PiecesMap[FVector2D(x, y)];

		if (SelectedPiece && SelectedPiece->Color == (IsHumanPlayer ? EPieceColor::BLACK : EPieceColor::WHITE))
		{
			if (!SelectedPiece->IsA<AKing>())
			{
				Field->Support.Add(SelectedPiece);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Ho trovato il re passando per %d "), Field->Support.Num());
				//Alla fine qua avro tutti i pezzi che devono muoversi in una dir specifica
				if (Field->Support.Num() == 1)
					Field->StoragePiece.Add(Field->Support[0]);
			}
		}
		else
			Marked = true;
	}

}

void AChessPieces::PieceDestroy()
{
	Destroy();
}