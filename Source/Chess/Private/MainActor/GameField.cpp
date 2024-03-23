// Fill out your copyright notice in the Description page of Project Settings.

#include "../Public/MainActor/GameField.h"
#include "Pieces/King.h"
#include "../Public/Manager/Chess_GameMode.h"

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
}

void AGameField::ResetField()
{
	auto GMode = FGameRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null in GameField"));
	}
	auto ManagerPiece = GMode->Manager;
	if (ManagerPiece)
	{
		for (int32 i = 0; i < ManagerPiece->CapturedPieces.Num(); i++)
		{
			ManagerPiece->CapturedPieces[i]->Destroy();
			//ManagerPiece->CapturedPieces[i] = nullptr;
		}
		ManagerPiece->CapturedPieces.Empty();

		for (int32 i = 0; i < ManagerPiece->PromotePieces.Num(); i++)
		{
			ManagerPiece->PromotePieces[i]->Destroy();
		}
		ManagerPiece->PromotePieces.Empty();

		for (int32 i = 0; i < ManagerPiece->ArrayOfPlays.Num(); i++)
		{
			ManagerPiece->ArrayOfPlays[i].PieceToRewind->Destroy();
			//ManagerPiece->ArrayOfPlays = nullptr;
		}
		ManagerPiece->ArrayOfPlays.Empty();

		for (int32 i = 0; i < ManagerPiece->LegalMoveArray.Num(); i++)
			ManagerPiece->LegalMoveArray[i].Empty();
		ManagerPiece->LegalMoveArray.Empty();

		ManagerPiece->Count = 1;
	}
	Cont = 0;
	ResetAll();

	GenerateField();
	OnResetEvent.Broadcast();
	
	GMode->ChoosePlayerAndStartGame();
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

void AGameField::BeginDestroy()
{
	Super::BeginDestroy();
}

void AGameField::GenerateField()
{
	int32 i = 0;
	for (int32 x = 0; x < Size; x++)
	{
		for (int32 y = 0; y < Size; y++)
		{
			// spawn black tile followed by white tile
			GenerateTileInXYPosition(x, y, GameFieldSubClass.TileClass[i]);
			!i ? ++i : i = 0;
		}
		// swap the white tile with the black tile every row
		x%2 ? i = 0 : i = 1;
	}

	// Spawn chess pieces human player
	int k = 0;
	int normalized_row = 0;

	const TArray<TSubclassOf<AChessPieces>> WHITE_PIECE = { GameFieldSubClass.ChessRook[0], GameFieldSubClass.ChessKnight[0], 
															GameFieldSubClass.ChessBishop[0], GameFieldSubClass.ChessQueen[0], 
															GameFieldSubClass.ChessKing[0], GameFieldSubClass.ChessBishop[0],
															GameFieldSubClass.ChessKnight[0], GameFieldSubClass.ChessRook[0] };

	const TArray<TSubclassOf<AChessPieces>> BLACK_PIECE = { GameFieldSubClass.ChessRook[1], GameFieldSubClass.ChessKnight[1], 
															GameFieldSubClass.ChessBishop[1], GameFieldSubClass.ChessQueen[1], 
															GameFieldSubClass.ChessKing[1], GameFieldSubClass.ChessBishop[1],
															GameFieldSubClass.ChessKnight[1],GameFieldSubClass.ChessRook[1] };
	for (int32 x = 0; x < SECOND_ROW_FIELD; x++)
	{
		for (int32 y = 0; y < Size; y++)
		{
			k = y + normalized_row;
			if (k < 8) 
			{
				// generate the chess pieces in the first row
				GenerateChessPieceInXYPosition(x, y, WHITE_PIECE[y], EPieceColor::WHITE);
			}
			else
			{
				// generate the chess pieces in the second row
				GenerateChessPieceInXYPosition(x, y, GameFieldSubClass.ChessPawn[Player::HUMAN], EPieceColor::WHITE);
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
				GenerateChessPieceInXYPosition(x, y, GameFieldSubClass.ChessPawn[Player::AI], EPieceColor::BLACK);
			}
			else
			{
				// generate the chess pieces in the second row
				GenerateChessPieceInXYPosition(x, y, BLACK_PIECE[y], EPieceColor::BLACK);
			}
		}
		normalized_row = 24;
	}
}

// generate the tile 
void AGameField::GenerateTileInXYPosition(int32 x, int32 y, TSubclassOf<ATile> Class)
{
	TArray<FString> IntToChar = { "a", "b", "c", "d", "e", "f", "g", "h" };

	FVector Location = AGameField::GetRelativeLocationByXYPosition(x, y);
	ATile* Obj = GetWorld()->SpawnActor<ATile>(Class, Location, FRotator(0.0f, 0.0f, 0.0f));
	const float TileScale = TileSize / 100;
	Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
	Obj->SetGridPosition(x, y);
	Obj->Name = IntToChar[y] + FString::FromInt(x+1);
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
	Obj->IndexArray = Cont;
	Cont++;

	ATile* CurrTile = nullptr;
	if (TileMap.Contains(FVector2D(x, y)))
		CurrTile = TileMap[FVector2D(x, y)];
	if (CurrTile)
		color == EPieceColor::BLACK ? CurrTile->SetTileStatus(1, ETileStatus::OCCUPIED) : CurrTile->SetTileStatus(0, ETileStatus::OCCUPIED);

	PiecesMap.Add(FVector2D(x, y), Obj);
	if (color == EPieceColor::BLACK)
		BotPieces.Add(Obj);
	else
		HumanPlayerPieces.Add(Obj);

	if (Class == GameFieldSubClass.ChessKing[0])
		KingArray.Add(Cast<AKing>(Obj));
	if (Class == GameFieldSubClass.ChessKing[1])
		KingArray.Add(Cast<AKing>(Obj));

	FRewind NewObj;
	NewObj.PieceToRewind = Obj;
	NewObj.Position = Obj->GetGridPosition();
	NewObj.Capture = false;

	auto GMode = FGameRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null in GameField"));
	}
	auto ManagerPiece = GMode->Manager;

	if (ManagerPiece)
	{
		ManagerPiece->ArrayOfPlays.Add(NewObj);
		ManagerPiece->LegalMoveArray.SetNum(Cont);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Manager Piece null in GameField"));
}

FVector2D AGameField::GetPosition(const FHitResult& Hit)
{
	return Cast<ATile>(Hit.GetActor())->GetGridPosition();
}

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