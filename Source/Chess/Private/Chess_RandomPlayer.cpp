// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_RandomPlayer.h"
#include "ChessPieces.h"
#include "GameField.h"
#include "Tile.h"

// Sets default values
AChess_RandomPlayer::AChess_RandomPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//GameInstance = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	PieceColor = EPieceColor::WHITE;
}

// Called when the game starts or when spawned
void AChess_RandomPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChess_RandomPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AChess_RandomPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AChess_RandomPlayer::OnTurn()
{
	//UE_LOG(LogTemp, Warning, TEXT("Turno bot!"));
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Turn"));
	//GameInstance->SetTurnMessage(TEXT("AI (Random) Turn"));

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			AChess_GameMode* GMode = Cast<AChess_GameMode>(GWorld->GetAuthGameMode());
			TArray<AChessPieces*> PiecesArray = GMode->GField->BotPieces;
			bool PieceIsPossibleToMove = false;
			int32 RIndex;

			do{
				RIndex = FMath::Rand() % 16;
				AChessPieces* CurrPiece = PiecesArray[RIndex];
				CurrPiece->LegalMove(PlayerNumber, false);
				if (CurrPiece->TileMarked.Num() == 0)
					continue;
				PieceIsPossibleToMove = true;
				int32 RIndexToMovePiece = FMath::Rand() % CurrPiece->TileMarked.Num();
				ATile* TileActor = CurrPiece->TileMarked[RIndexToMovePiece];
				FVector SpawnPosition = TileActor->GetActorLocation();
				TileActor->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);
				AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
				if (CurrPiece == nullptr)
					UE_LOG(LogTemp, Error, TEXT("PieceToSpawn or PieceToMove cannot be null"));
				FVector2D Coord = TileActor->GetGridPosition();
				GameMode->MovePiece(PlayerNumber, SpawnPosition, CurrPiece, Coord);
			} while (!PieceIsPossibleToMove);

		}, 3, false);
}

void AChess_RandomPlayer::OnWin()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Wins!"));
	//GameInstance->SetTurnMessage(TEXT("AI Wins!"));
	//GameInstance->IncrementScoreAiPlayer();
}

void AChess_RandomPlayer::OnLose()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Loses!"));
	// GameInstance->SetTurnMessage(TEXT("AI Loses!"));
}