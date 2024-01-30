// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_GameMode.h"
#include "Chess_PlayerController.h"
#include "ChessPieces.h"
#include "Chess_HumanPlayer.h"
#include "Chess_RandomPlayer.h"
#include "EngineUtils.h"
#include "Engine/World.h"

AChess_GameMode::AChess_GameMode()
{
	PlayerControllerClass = AChess_PlayerController::StaticClass();

	DefaultPawnClass = AChess_HumanPlayer::StaticClass();
	static ConstructorHelpers::FClassFinder<AGameField> BlueprintClassFinder(TEXT("/Game/Path/To/BP_GameField"));
	if (BlueprintClassFinder.Succeeded())
	{
		GameFieldClass = BlueprintClassFinder.Class;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find Blueprint class!"));
	}

	FieldSize = 8;
}

void AChess_GameMode::BeginPlay()
{
	Super::BeginPlay();

	IsGameOver = false;

	//MoveCounter = 0;

	AChess_HumanPlayer* HumanPlayer = Cast<AChess_HumanPlayer>(*TActorIterator<AChess_HumanPlayer>(GetWorld()));

	if (GameFieldClass != nullptr)
	{
		GField = GetWorld()->SpawnActor<AGameField>(GameFieldClass);
		GField->Size = FieldSize;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Game Field is null"));
	}

	float CameraPosX = ((GField->TileSize * (FieldSize + ((FieldSize - 1) * GField->NormalizedCellPadding) - (FieldSize - 1))) / 2) - (GField->TileSize / 2);
	FVector CameraPos(CameraPosX, CameraPosX, 1000.0f);
	HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());

	Players.Add(HumanPlayer);
	// Random Player
	auto* AI = GetWorld()->SpawnActor<AChess_RandomPlayer>(FVector(), FRotator());

	// MiniMax Player
	//auto* AI = GetWorld()->SpawnActor<ATTT_MinimaxPlayer>(FVector(), FRotator());

	// AI player = 1
	Players.Add(AI);

	this->ChoosePlayerAndStartGame();
}

void AChess_GameMode::ChoosePlayerAndStartGame()
{
	
	//CurrentPlayer = FMath::RandRange(0, Players.Num() - 1);

	CurrentPlayer = 0;
	for (int32 i = 0; i < Players.Num(); i++)
	{
		Players[i]->PlayerNumber = i;
		Players[i]->PieceColor = i == i == CurrentPlayer ? EPieceColor::BLACK : EPieceColor::WHITE;
	}
	//MoveCounter += 1;
	Players[CurrentPlayer]->OnTurn();
}

void AChess_GameMode::MovePiece(const int32 PlayerNumber, const FVector& SpawnPosition, UClass* PieceToSpawn, FVector2D NewCoordToSpawnPiece, AChessPieces* PieceToRemove)
{
	if (IsGameOver || PlayerNumber != CurrentPlayer)
	{
		return;
	}
	
	FVector NewLocation = GField->GetActorLocation() + SpawnPosition;
	//PieceToRemove->SetGridPosition(NewCoordToSpawnPiece[0], NewCoordToSpawnPiece[1]);
	//GetWorld()->SpawnActor(PieceToRemove, &NewLocation);
	
	//int32 IndexElementToRemove = GField->PiecesArray.Find(PieceToRemove);
	//GField->PiecesArray[IndexElementToRemove]->Destroy();
	//GField->PiecesMap.Remove(FVector2D(PieceToRemove->GetGridPosition()[0], PieceToRemove->GetGridPosition()[1]));
	//GetWorld()->RemoveActor(PieceToRemove, true);
	//AChessPieces* PieceInNewPosition = Cast<AChessPieces>(GetWorld()->SpawnActor(PieceToSpawn, &NewLocation));
	//PieceInNewPosition->SetActorRotation(FRotator(0.0f, 90.0f, 0.0f));
	//PieceInNewPosition->SetGridPosition(NewCoordToSpawnPiece[0], NewCoordToSpawnPiece[1]);
	//GField->PiecesArray.Add(PieceInNewPosition);
	//GField->PiecesMap.Add(FVector2D(NewCoordToSpawnPiece[0], NewCoordToSpawnPiece[1]), Cast<AChessPieces>(PieceToSpawn));
	//TODO: win case

	TurnNextPlayer();
}

int32 AChess_GameMode::GetNextPlayer(int32 Player)
{
	Player++;
	if (!Players.IsValidIndex(Player))
		Player = 0;
	return Player;
}

void AChess_GameMode::TurnNextPlayer()
{
	//MoveCounter += 1;
	CurrentPlayer = GetNextPlayer(CurrentPlayer);
	Players[CurrentPlayer]->OnTurn();
}