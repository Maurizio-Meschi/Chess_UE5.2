// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_GameMode.h"
#include "Chess_PlayerController.h"
#include "ChessPieces.h"
#include "Chess_HumanPlayer.h"
#include "Chess_RandomPlayer.h"
#include "Chess_GameInstance.h"
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

	MoveCounter = 0;
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
		Players[i]->PieceColor = i == i == CurrentPlayer ? EPieceColor::WHITE : EPieceColor::BLACK;
	}
	//MoveCounter += 1;
	Players[CurrentPlayer]->OnTurn();
}

void AChess_GameMode::MovePiece(const int32 PlayerNumber, const FVector& SpawnPosition, AChessPieces* Piece, FVector2D Coord)
{
	if (IsGameOver || PlayerNumber != CurrentPlayer)
	{
		return;
	}
	FVector NewLocation = GField->GetActorLocation() + SpawnPosition;

	CriticalSection.Lock();

	UE_LOG(LogTemp, Error, TEXT("Ho tolto il pezzo nella vecchia posizione"));
	GField->PiecesMapRemove(FVector2D(Piece->GetGridPosition().X, Piece->GetGridPosition().Y));
	Piece->SetGridPosition(Coord.X, Coord.Y);
	
	GField->AddPiecesMap(Coord, Piece);
	Piece->SetActorLocation(NewLocation);

	CriticalSection.Unlock();

	// Add the piece reference in the current played 
	UChess_GameInstance* GInstance = Cast<UChess_GameInstance>(GetWorld()->GetAuthGameMode());
	FRewind Obj;
	Obj.PieceToRewind = Piece;  
	Obj.Position = Piece->GetGridPosition();
	ArrayOfPlays.Add(Obj);

	//TODO: win case

	TurnNextPlayer();
}

void AChess_GameMode::CapturePiece(AChessPieces* PieceToCapture, FVector2D Coord)
{
	CriticalSection.Lock();
	UE_LOG(LogTemp, Error, TEXT("Ho tolto il pezzo catturato dalla mappa"));
	GField->PiecesMapRemove(Coord);
	//GField->PiecesArray.Remove(PieceToCapture);
	if (PieceToCapture->Color == EPieceColor::BLACK)
	{
		GField->BotPiecesRemove(PieceToCapture);
	}

	CriticalSection.Unlock();

	PieceToCapture->SetActorHiddenInGame(true);
	PieceToCapture->SetActorEnableCollision(false);
	//PieceToCapture->PieceDestroy();
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