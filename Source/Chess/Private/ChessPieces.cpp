// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessPieces.h"
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

	if (SelectedTile->GetTileStatus() == ETileStatus::EMPTY)
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

void AChessPieces::PieceDestroy()
{
	Destroy();
}