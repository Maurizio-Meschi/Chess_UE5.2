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
				// select random piece
				RIndex = FMath::Rand() % PiecesArray.Num();
				AChessPieces* CurrPiece = PiecesArray[RIndex];
				// check the possible move
				CurrPiece->LegalMove(PlayerNumber, false);

				if (CurrPiece->TileMarked.Num() == 0)
					continue;

				PieceIsPossibleToMove = true;

				// select the marked tile to move
				int32 RIndexToMovePiece = FMath::Rand() % CurrPiece->TileMarked.Num();

				// take the tile where move the piece
				ATile* TileActor = CurrPiece->TileMarked[RIndexToMovePiece];
				FVector SpawnPosition = TileActor->GetActorLocation();

				AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());

				FVector2D Coord = TileActor->GetGridPosition();
				// check if possible to capture an enemy piece
				if (TileActor->GetTileStatus() == ETileStatus::MARKED_TO_CAPTURE)
				{
					TileActor->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);
					AChessPieces* PieceToCapture = GMode->GField->PiecesMap[(Coord)];
					GameMode->CapturePiece(PieceToCapture, Coord);
				}
				TileActor->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);
				// Before moving the piece, set the current tile to be empty
				GMode->GField->TileMap[FVector2D(CurrPiece->GetGridPosition()[0], CurrPiece->GetGridPosition()[1])]->SetTileStatus(PlayerNumber, ETileStatus::EMPTY);

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