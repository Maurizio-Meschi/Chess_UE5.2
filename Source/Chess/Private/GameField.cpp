// Fill out your copyright notice in the Description page of Project Settings.

#include "GameField.h"
#include "Chess_GameMode.h"

// Sets default values
AGameField::AGameField()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	// size of the field (3x3)
	Size = 8;
	// tile dimension
	TileSize = 120;
	// tile padding dimension
	CellPadding = 5;
	// one black piece and one white piece
	TileClass.SetNum(2);
	ChessRook.SetNum(2);
	ChessKing.SetNum(2);
	ChessKnight.SetNum(2);
	ChessQueen.SetNum(2);
	ChessBishop.SetNum(2);
	ChessPawn.SetNum(2);
}

void AGameField::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	//normalized tilepadding
	NormalizedCellPadding = FMath::RoundToDouble(((TileSize + CellPadding) / TileSize) * 100) / 100;
}

// Called when the game starts or when spawned
void AGameField::BeginPlay()
{
	Super::BeginPlay();
	GenerateField();
}

void AGameField::GenerateField()
{
	int32 i = 0;
	for (int32 x = 0; x < Size; x++)
	{
		for (int32 y = 0; y < Size; y++)
		{
			// spawn black tile followed by white tile
			GenerateTileInXYPosition(x, y, TileClass[i]);
			!i ? ++i : i = 0;
		}
		// swap the white tile with the black tile every row
		x%2 ? i = 0 : i = 1;
	}

	// Spawn chess pieces human player
	int k = 0;
	int normalized_row = 0;
	TArray<TSubclassOf<AChessPieces>> Type;
	for (int32 x = 0; x < SECOND_ROW_FIELD; x++)
	{
		for (int32 y = 0; y < Size; y++)
		{
			k = y + normalized_row;
			if (k < 8) 
			{
				// generate the chess pieces in the first row
				Type = { ChessRook[0], ChessKnight[0], ChessBishop[0], ChessQueen[0], ChessKing[0], ChessBishop[0], ChessKnight[0], ChessRook[0] };
				GenerateChessPieceInXYPosition(x, y, Type[y], EPieceColor::BLACK);
			}
			else
			{
				// generate the chess pieces in the second row
				GenerateChessPieceInXYPosition(x, y, ChessPawn[0], EPieceColor::BLACK);
			}
		}
		normalized_row = 8;
	}

	// Spawn chess pieces bot palyer
	k = 0;
	normalized_row = 16;
	
	for (int32 x = PENULTIMATE_ROW_FIELD; x < LAST_ROW_FIELD; x++)
	{
		for (int32 y = 0; y < Size; y++)
		{
			k = y + normalized_row;
			if (k < 24)
			{
				// generate the chess pieces in the first row
				GenerateChessPieceInXYPosition(x, y, ChessPawn[1], EPieceColor::WHITE);
			}
			else
			{
				// generate the chess pieces in the second row
				Type = { ChessRook[1], ChessKnight[1], ChessBishop[1], ChessQueen[1], ChessKing[1], ChessBishop[1], ChessKnight[1], ChessRook[1] };
				GenerateChessPieceInXYPosition(x, y, Type[y], EPieceColor::WHITE);
			}
		}
		normalized_row = 24;
	}
	
	for (int32 w = 16; w < 32; w++)
	{
		BotPieces.Add(PiecesArray[w]);
	}
}

// generate the tile 
void AGameField::GenerateTileInXYPosition(int32 x, int32 y, TSubclassOf<ATile> Class)
{
	FVector Location = AGameField::GetRelativeLocationByXYPosition(x, y);
	ATile* Obj = GetWorld()->SpawnActor<ATile>(Class, Location, FRotator(0.0f, 0.0f, 0.0f));
	const float TileScale = TileSize / 100;
	Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
	Obj->SetGridPosition(x, y);
	TileArray.Add(Obj);
	TileMap.Add(FVector2D(x, y), Obj);
}

// generate the chess piece 
void AGameField::GenerateChessPieceInXYPosition(int32 x, int32 y, TSubclassOf<AChessPieces> Class, EPieceColor color)
{
	FVector Location = AGameField::GetRelativeLocationByXYPosition(x, y);
	AChessPieces* Obj = GetWorld()->SpawnActor<AChessPieces>(Class, Location, FRotator(0.0f, 90.0f, 0.0f));
	const float TileScale = TileSize / 100;
	Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
	Obj->SetGridPosition(x, y);
	Obj->SetColor(color);
	ATile* CurrTile = TileMap[FVector2D(x, y)];
	color == EPieceColor::BLACK ? CurrTile->SetTileStatus(0, ETileStatus::OCCUPIED) : CurrTile->SetTileStatus(1, ETileStatus::OCCUPIED);
	PiecesArray.Add(Obj);
	PiecesMap.Add(FVector2D(x, y), Obj);
}

FVector2D AGameField::GetPosition(const FHitResult& Hit)
{
	return Cast<ATile>(Hit.GetActor())->GetGridPosition();
}

TArray<ATile*> AGameField::GetTileArray()
{
	return TileArray;
}

TArray<AChessPieces*>& AGameField::GetPiecesArray()
{
	return PiecesArray;
}

// get the space position
FVector AGameField::GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const
{
	return TileSize * NormalizedCellPadding * FVector(InX, InY, 0);
}

FVector2D AGameField::GetXYPositionByRelativeLocation(const FVector& Location) const
{
	const double x = Location[0] / (TileSize * NormalizedCellPadding);
	const double y = Location[1] / (TileSize * NormalizedCellPadding);
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("x=%f,y=%f"), x, y));
	return FVector2D(x, y);
}

void AGameField::ResetTileMarked()
{
	for (int32 i = 0; i < TileMarked.Num(); i++)
	{
		if (TileMarked[i]->GetTileStatus() == ETileStatus::MARKED)
			TileMarked[i]->SetTileStatus(-1, ETileStatus::EMPTY);

		if (TileMarked[i]->GetTileStatus() == ETileStatus::MARKED_TO_CAPTURE)
			TileMarked[i]->SetTileStatus(1, ETileStatus::OCCUPIED);
	}
	TileMarked.Empty();
}

inline bool AGameField::IsValidPosition(const FVector2D Position) const
{
	return 0 <= Position[0] && Position[0] < Size && 0 <= Position[1] && Position[1] < Size;
}
