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

	PieceIndexValue = 0;
}


/*
* @param: none
* @return: none
* @note: reset all data structures and restart the game
*/
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
		ManagerPiece->ResetData();
		ManagerPiece->DeleteTime();
	}
	PieceIndexValue = 0;
	GMode->FEN_Array.Empty();
	ResetFieldData();

	GenerateField();
	
	auto PlayerController = Cast<AChess_PlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	if (PlayerController)
		PlayerController->EnableInput(PlayerController);

	GMode->ChoosePlayerAndStartGame();
}

void AGameField::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	//normalized tilepadding
	NormalizedCellPadding = FMath::RoundToDouble(((TileSize + CellPadding) / TileSize) * 100) / 100;
}


void AGameField::BeginPlay()
{
	Super::BeginPlay();
	GenerateField();
}

/*
* @param: none
* @return: none
* @note: Manages the chessboard spawn in three steps:
*          1.) spawn tiles
*          2.) spawn white pieces
*		   3.) spawn black pieces
*/
void AGameField::GenerateField()
{
	// 1.) Spawn tiles
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

	// 
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

	// 2.) Spawn chess pieces human player (white)
	for (int32 x = 0; x < SECOND_ROW_FIELD; x++)
	{
		for (int32 y = 0; y < Size; y++)
		{
			k = y + normalized_row;
			if (k < 8) 
			{
				// Generate the chess pieces in the first row
				GenerateChessPieceInXYPosition(x, y, WHITE_PIECE[y], EPieceColor::WHITE);
			}
			else
			{
				// Generate the chess pieces in the second row
				GenerateChessPieceInXYPosition(x, y, GameFieldSubClass.ChessPawn[Player::Player1], EPieceColor::WHITE);
			}
		}
		normalized_row = 8;
	}

	// 2.) Spawn chess pieces AI player (balck)
	k = 0;
	normalized_row = 16;
	
	for (int32 x = PENULTIMATE_ROW_FIELD; x < LAST_ROW_FIELD; x++)
	{
		for (int32 y = 0; y < Size; y++)
		{
			k = y + normalized_row;
			if (k < 24)
			{
				// Generate the chess pieces in the first row
				GenerateChessPieceInXYPosition(x, y, GameFieldSubClass.ChessPawn[Player::AI], EPieceColor::BLACK);
			}
			else
			{
				// Generate the chess pieces in the second row
				GenerateChessPieceInXYPosition(x, y, BLACK_PIECE[y], EPieceColor::BLACK);
			}
		}
		normalized_row = 24;
	}
}

/*
* @param: 1.) X position
*         2.) Y position
*         3.) Tile class
* 
* @return: none
* 
* @note: spawn the tile, sets its values and add it in the tile map
*/
void AGameField::GenerateTileInXYPosition(int32 x, int32 y, TSubclassOf<ATile> Class)
{
	// The y position of the tile is encoded with a letter on the board
	TArray<FString> IntToChar = { "a", "b", "c", "d", "e", "f", "g", "h" };

	FVector Location = AGameField::GetRelativeLocationByXYPosition(x, y);
	ATile* Obj = GetWorld()->SpawnActor<ATile>(Class, Location, FRotator(0.0f, 0.0f, 0.0f));
	const float TileScale = TileSize / 100;
	Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
	Obj->SetGridPosition(x, y);
	Obj->Name = IntToChar[y] + FString::FromInt(x+1);

	TileMap.Add(FVector2D(x, y), Obj);
}


/*
* @param: 1.) X position
*         2.) Y position
*         3.) Piece class
*         4.) Piece Color
*
* @return: none
*
* @note: spawn the piece, sets its values and add it in data structures
*/
void AGameField::GenerateChessPieceInXYPosition(int32 x, int32 y, TSubclassOf<AChessPieces> Class, EPieceColor color)
{
	FVector Location = AGameField::GetRelativeLocationByXYPosition(x, y);
	AChessPieces* Obj = GetWorld()->SpawnActor<AChessPieces>(Class, Location, FRotator(0.0f, 90.0f, 0.0f));
	const float TileScale = TileSize / 100;
	Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
	Obj->SetGridPosition(x, y);
	Obj->SetColor(color);
	Obj->IndexArray = PieceIndexValue;
	PieceIndexValue++;

	ATile* CurrTile = nullptr;
	if (TileMap.Contains(FVector2D(x, y)))
		CurrTile = TileMap[FVector2D(x, y)];
	if (CurrTile)
		color == EPieceColor::BLACK ? CurrTile->SetTileStatus(1, ETileStatus::OCCUPIED) : CurrTile->SetTileStatus(0, ETileStatus::OCCUPIED);

	PiecesMap.Add(FVector2D(x, y), Obj);

	if (color == EPieceColor::BLACK)
		AIPieces.Add(Obj);
	else
		Player1Pieces.Add(Obj);

	if (Class == GameFieldSubClass.ChessKing[0])
		KingArray.Add(Cast<AKing>(Obj));
	if (Class == GameFieldSubClass.ChessKing[1])
		KingArray.Add(Cast<AKing>(Obj));

	// Add the starting move to the ArrayOfPlays
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
		ManagerPiece->LegalMoveArray.SetNum(PieceIndexValue);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Manager Piece null in GameField"));
}

FVector AGameField::GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const
{
	return TileSize * NormalizedCellPadding * FVector(InX, InY, 0);
}

FVector2D AGameField::GetXYPositionByRelativeLocation(const FVector& Location) const
{
	const double x = Location[0] / (TileSize * NormalizedCellPadding);
	const double y = Location[1] / (TileSize * NormalizedCellPadding);
	
	return FVector2D(x, y);
}