// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_GameMode.h"
#include "Chess_PlayerController.h"
#include "ChessPieces.h"
#include "Chess_HumanPlayer.h"
#include "Chess_RandomPlayer.h"
#include "Chess_GameInstance.h"
#include "Async/TaskGraphInterfaces.h"
#include "Async/TaskGraphInterfaces.h"
#include "EngineUtils.h"
#include "Engine/World.h"

AChess_GameMode::AChess_GameMode()
{
	DefaultPawnClass = AChess_HumanPlayer::StaticClass();

	FieldSize = 8;

	MoveCounter = 0;
}

void AChess_GameMode::BeginPlay()
{
	Super::BeginPlay();

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

	if (ManagerClass)
	{
		Manager = GetWorld()->SpawnActor<AManagePiece>(ManagerClass);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Manager is null"));

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

void AChess_GameMode::BeginDestroy()
{
	Super::BeginDestroy();
	
	//Players.Empty();
	//GField = nullptr;
}

void AChess_GameMode::ChoosePlayerAndStartGame()
{
	
	//CurrentPlayer = FMath::RandRange(0, Players.Num() - 1);

	CurrentPlayer = 0;

	for (int32 i = 0; i < Players.Num(); i++)
	{
		Players[i]->PlayerNumber = i;
		Players[i]->PieceColor = i == i == CurrentPlayer ? EPieceColor::WHITE : EPieceColor::BLACK;
	}
	//MoveCounter += 1;
	Players[CurrentPlayer]->OnTurn();
}

int32 AChess_GameMode::GetNextPlayer(int32 Player)
{
	Player++;
	MoveCounter++;
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

AChess_GameMode* FGameModeRef::CachedGameMode = nullptr;

AChess_GameMode* FGameModeRef::GetGameMode(UObject* WorldContextObject)
{
	if (!CachedGameMode)
	{
		UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
		CachedGameMode = World->GetAuthGameMode<AChess_GameMode>();
	}

	return CachedGameMode;
}
