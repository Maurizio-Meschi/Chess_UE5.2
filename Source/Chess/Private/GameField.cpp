// Fill out your copyright notice in the Description page of Project Settings.

#include "GameField.h"

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

	ChessPieces.SetNum(32);
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
	for (int32 x = 0; x < Size; x++)
	{
		for (int32 y = 0; y < Size; y += 2)
		{
			// In each row the order in which the tiles are generated is exchanged
			if (x % 2)
			{
				// Generate the first tile
				GenerateTileInXYPosition(x, y, TileClass1);
				// Generate the second tile
				GenerateTileInXYPosition(x, y + 1, TileClass2);
			}
			else
			{
				GenerateTileInXYPosition(x, y + 1, TileClass1);
				GenerateTileInXYPosition(x, y, TileClass2);
			}
		}
	}

	// Spawn chess pieces human player
	int k = 0;
	int normalized_row = 0;
	for (int32 x = 0; x < SECOND_ROW_FIELD; x++)
	{
		for (int32 y = 0; y < 8; y++)
		{
			k = y + normalized_row;
			GenerateTileInXYPosition(x, y, ChessPieces[k]);
		}
		normalized_row = 8;
	}

	// Spawn chess pieces bot palyer
	k = 0;
	normalized_row = 16;
	for (int32 x = PENULTIMATE_ROW_FIELD; x < LAST_ROW_FIELD; x++)
	{
		for (int32 y = 0; y < 8; y++)
		{
			k = y + normalized_row;
			GenerateTileInXYPosition(x, y, ChessPieces[k]);
		}
		normalized_row = 24;
	}
}

void AGameField::GenerateTileInXYPosition(int32 x, int32 y, TSubclassOf<ATile> TileClass)
{
	FVector Location = AGameField::GetRelativeLocationByXYPosition(x, y);
	ATile* Obj = GetWorld()->SpawnActor<ATile>(TileClass, Location, FRotator(0.0f, 90.0f, 0.0f));
	const float TileScale = TileSize / 100;
	Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
	Obj->SetGridPosition(x, y);
	TileArray.Add(Obj);
	TileMap.Add(FVector2D(x, y), Obj);
}

FVector2D AGameField::GetPosition(const FHitResult& Hit)
{
	return Cast<ATile>(Hit.GetActor())->GetGridPosition();
}

TArray<ATile*>& AGameField::GetTileArray()
{
	return TileArray;
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

inline bool AGameField::IsValidPosition(const FVector2D Position) const
{
	return 0 <= Position[0] && Position[0] < Size && 0 <= Position[1] && Position[1] < Size;
}