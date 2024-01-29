// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_GameMode.h"
#include "Chess_HumanPlayer.h"
#include "Chess_RandomPlayer.h"
#include "EngineUtils.h"

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

	//this->ChoosePlayerAndStartGame();
}

void AChess_GameMode::ChoosePlayerAndStartGame()
{
	/*
	CurrentPlayer = FMath::RandRange(0, Players.Num() - 1);

	//CurrentPlayer = 0;
	for (int32 i = 0; i < Players.Num(); i++)
	{
		Players[i]->PlayerNumber = i;
		//Players[i]->Sign = i == CurrentPlayer ? ESign::X : ESign::O;
	}
	//MoveCounter += 1;
	Players[CurrentPlayer]->OnTurn();
	*/
}

