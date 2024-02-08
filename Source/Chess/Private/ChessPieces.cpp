// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessPieces.h"
#include "Pieces/ChessPawn.h"
#include "GameField.h"
#include "Tile.h"
#include "Chess_GameMode.h"

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
	AChess_GameMode* GameMOde = Cast<AChess_GameMode>(GWorld->GetAuthGameMode());
	//GameMode->GField->OnResetEvent.AddDynamic(this, &ABaseSign::SelfDestroy);
}

bool AChessPieces::CheckCoord(int32 x, int32 y)
{
	if (x < 0 || x > 7 || y < 0 || y > 7) return false;
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
	if (GameModeClass != nullptr)
		GMode = Cast<AChess_GameMode>(GWorld->GetAuthGameMode());
	else
		UE_LOG(LogTemp, Error, TEXT("Game Mode is null"));

	AGameField* Field = GMode->GField;

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

	ATile* SelectedTile = nullptr;

	GMode->CriticalSection.Lock();

	SelectedTile = TileMap[FVector2D(x, y)];

	GMode->CriticalSection.Unlock();
	if (this->GetClass()->GetName() == (IsHumanPlayer ? "BP_w_King_C" : "BP_b_King_C") &&
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
		GMode->CriticalSection.Lock();

		AChessPieces* SelectedPiece = PiecesMap[FVector2D(x, y)];

		GMode->CriticalSection.Unlock();

		if (SelectedPiece->Color == (IsHumanPlayer ? EPieceColor::BLACK : EPieceColor::WHITE))
		{
			if (SelectedPiece->GetClass()->GetName() != (IsHumanPlayer ? "BP_b_King_C" : "BP_w_King_C"))
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
	GMode = Cast<AChess_GameMode>(GWorld->GetAuthGameMode());
	
	AGameField* Field = GMode->GField;

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

	ATile* SelectedTile = nullptr;

	GMode->CriticalSection.Lock();
	SelectedTile = TileMap[FVector2D(x, y)];
	GMode->CriticalSection.Unlock();

	if (Field->IsCheckmateSituation)
	{
		ManageCheckMateSituation(PlayerNumber, IsHumanPlayer, Marked, SelectedTile);
	}
	else
	{
		// Questa è valida solo per il king
		if (IsKing && SelectedTile->GetTileStatus() == ETileStatus::EMPTY)
		{
			SelectedTile->SEtStatusCheckmate(PlayerNumber, EStatusCheckmate::MARK_BY_KING);

		}
		// Tentativo per evitare che catturando una pedina mi vada ad esporre
		if (IsKing && SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
		{
			GMode->CriticalSection.Lock();
			AChessPieces* SelectedPiece = PiecesMap[FVector2D(x, y)];
			GMode->CriticalSection.Unlock();

			if (SelectedPiece->Color == (IsHumanPlayer ? EPieceColor::BLACK : EPieceColor::WHITE))
			{
				if (SelectedPiece->GetClass()->GetName() != (IsHumanPlayer ? "BP_b_King_C" : "BP_w_King_C"))
				{
					SelectedTile->SEtStatusCheckmate(PlayerNumber, EStatusCheckmate::MARK_BY_KING);
				}
			}
		}

		// controllo se una pedina può andare diretta contro il re
		else if (SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
		{
			GMode->CriticalSection.Lock();
			AChessPieces* SelectedPiece = PiecesMap[FVector2D(x, y)];
			GMode->CriticalSection.Unlock();

			if (SelectedPiece->Color == (IsHumanPlayer ? EPieceColor::BLACK : EPieceColor::WHITE))
			{
				if (SelectedPiece->GetClass()->GetName() == (IsHumanPlayer ? "BP_b_King_C" : "BP_w_King_C"))
				{
					Field->KingUnderAttack = true;
					FVector2d PiecePosition = this->GetGridPosition();
					GMode->CriticalSection.Lock();
					TileMap[PiecePosition]->SEtStatusCheckmate(PlayerNumber, EStatusCheckmate::CAPTURE_TO_AVOID_CHECKMATE);
					GMode->CriticalSection.Unlock();
					// Marca le tile per arrivare al re
					FindTileBetweenP1P2(PiecePosition, FVector2D(x, y), PlayerNumber);
				}
			}
			Marked = true;
		}
		// controllo se una pedina può andare in una cella in cui si potrebbe spostare il re
		if (!IsKing && SelectedTile->GetStatusCheckmate() == EStatusCheckmate::MARK_BY_KING)
		{
			SelectedTile->SEtStatusCheckmate(PlayerNumber, EStatusCheckmate::BLOCK_KING);
		}
	}
}

void AChessPieces::ManageCheckMateSituation(int32 PlayerNumber, bool IsHumanPlayer, bool& Marked, ATile* SelectedTile)
{
	GMode = Cast<AChess_GameMode>(GWorld->GetAuthGameMode());

	AGameField* Field = GMode->GField;

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

	if (SelectedTile->GetStatusCheckmate() == EStatusCheckmate::MARK_TO_AVOID_CHECKMATE)
	{
		SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED);
		Field->AddTileMarked(SelectedTile);
	}

	if (SelectedTile->GetStatusCheckmate() == EStatusCheckmate::MARK_AND_BLOCK_KING &&
		this->GetClass()->GetName() != (IsHumanPlayer ? "BP_w_King_C" : "BP_b_King_C"))
	{
		SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED);
		Field->AddTileMarked(SelectedTile);
	}

	// impedisco alle pedine di saltare i pezzi illegalmente
	if (SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
	{
		GMode->CriticalSection.Lock();
		AChessPieces* SelectedPiece = PiecesMap[SelectedTile->GetGridPosition()];
		GMode->CriticalSection.Unlock();

		//if (SelectedPiece->Color == (IsHumanPlayer ? EPieceColor::WHITE : EPieceColor::BLACK))
		//{
			Marked = true;
		//}
	}

	if (this->GetClass()->GetName() != (IsHumanPlayer ? "BP_w_King_C" : "BP_b_King_C") &&
		SelectedTile->GetStatusCheckmate() == EStatusCheckmate::CAPTURE_TO_AVOID_CHECKMATE)
	{
		SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED_TO_CAPTURE);
		Field->AddTileMarked(SelectedTile);
		Marked = true;
	}

	if (this->GetClass()->GetName() == (IsHumanPlayer ? "BP_w_King_C" : "BP_b_King_C")
		&& SelectedTile->GetStatusCheckmate() == EStatusCheckmate::MARK_BY_KING)
	{
		SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::MARKED);
		Field->AddTileMarked(SelectedTile);
	}
}

void AChessPieces::FindTileBetweenP1P2(const FVector2D& P1, const FVector2D& P2, int32 PlayerNumber)
{
	GMode = Cast<AChess_GameMode>(GWorld->GetAuthGameMode());
	AGameField* Field = GMode->GField;

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
			GMode->CriticalSection.Lock();
			UE_LOG(LogTemp, Error, TEXT("x: %d, y: %d"), x, y);
			SelectedTile = TileMap[FVector2D(x, y)];
			if (SelectedTile->GetStatusCheckmate() == EStatusCheckmate::MARK_BY_KING ||
				SelectedTile->GetStatusCheckmate() == EStatusCheckmate::BLOCK_KING)
				SelectedTile->SEtStatusCheckmate(PlayerNumber, EStatusCheckmate::MARK_AND_BLOCK_KING);
			else
				SelectedTile->SEtStatusCheckmate(PlayerNumber, EStatusCheckmate::MARK_TO_AVOID_CHECKMATE);

			GMode->CriticalSection.Unlock();
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

void AChessPieces::PieceDestroy()
{
	Destroy();
}