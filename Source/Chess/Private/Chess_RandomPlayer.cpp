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
			AGameField* Field = GMode->GField;
			TArray<AChessPieces*> PiecesArray = Field->BotPieces;
			bool PieceIsPossibleToMove = false;
			int32 RIndex;
			Field->ResetTileMarked();

			UE_LOG(LogTemp, Error, TEXT("Sono un bott!!"));

			do{
				// select random piece
				RIndex = FMath::Rand() % PiecesArray.Num();
				AChessPieces* CurrPiece = PiecesArray[RIndex];
				// check the possible move
				CurrPiece->LegalMove(PlayerNumber, false);

				if (Field->TileMarked.Num() == 0)
					continue;

				PieceIsPossibleToMove = true;

				// select the marked tile to move
				int32 RIndexToMovePiece = FMath::Rand() % Field->TileMarked.Num();

				// take the tile where move the piece
				ATile* TileActor = Field->TileMarked[RIndexToMovePiece];
				FVector SpawnPosition = TileActor->GetActorLocation();

				FVector2D Coord = TileActor->GetGridPosition();

				UE_LOG(LogTemp, Error, TEXT("E il momento di giocare!!"));

				// check if possible to capture an enemy piece
				if (TileActor->GetTileStatus() == ETileStatus::MARKED_TO_CAPTURE)
				{
					TileActor->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);
					AChessPieces* PieceToCapture = Field->PiecesMap[(Coord)];
					GMode->CapturePiece(PieceToCapture, Coord);
				}
				TileActor->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);

				int32 x = CurrPiece->GetGridPosition()[0];
				int32 y = CurrPiece->GetGridPosition()[1];
				UE_LOG(LogTemp, Error, TEXT("Prima di accedere alla map - Bot"));
				// Before moving the piece, set the current tile to be empty
				Field->TileMap[FVector2D(x, y)]->SetTileStatus(PlayerNumber, ETileStatus::EMPTY);

				GMode->MovePiece(PlayerNumber, SpawnPosition, CurrPiece, Coord);

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