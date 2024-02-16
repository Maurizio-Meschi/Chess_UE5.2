// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_RandomPlayer.h"
#include "ChessPieces.h"
#include "GameField.h"
#include "Tile.h"
#include "ManagePiece.h"

// Sets default values
AChess_RandomPlayer::AChess_RandomPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GameInstance = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	PieceManager = CreateDefaultSubobject<AManagePiece>(TEXT("PieceManager"));
	PieceColor = EPieceColor::BLACK;
}

// Called when the game starts or when spawned
void AChess_RandomPlayer::BeginPlay()
{
	Super::BeginPlay();
	//GMode = Cast<AChess_GameMode>(GWorld->GetAuthGameMode());
	//PieceManager = NewObject<AManagePiece>();
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
	GameInstance->SetTurnMessage(TEXT("AI (Random) Turn"));
	GameInstance->IncrementNumPlayed();
	
	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			auto GMode = FGameModeRef::GetGameMode(this);
			AGameField* Field = GMode->GField;

			TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
			TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

			TArray<AChessPieces*> PiecesArray = Field->GetBotPieces();

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

				TArray<ATile*> TileMarked = Field->GetTileMarked();

				if (TileMarked.Num() == 0)
					continue;

				PieceIsPossibleToMove = true;

				// select the marked tile to move
				int32 RIndexToMovePiece = FMath::Rand() % TileMarked.Num();

				// take the tile where move the piece
				ATile* TileActor = TileMarked[RIndexToMovePiece];
				FVector SpawnPosition = TileActor->GetActorLocation();

				FVector2D Coord = TileActor->GetGridPosition();

				UE_LOG(LogTemp, Error, TEXT("E il momento di giocare!!"));

				// check if possible to capture an enemy piece
				if (TileActor->GetTileStatus() == ETileStatus::MARKED_TO_CAPTURE)
				{
					TileActor->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);

					GMode->CriticalSection.Lock();

					AChessPieces* PieceToCapture = PiecesMap[(Coord)];

					GMode->CriticalSection.Unlock();

					PieceManager->CapturePiece(PieceToCapture, Coord);
				}
				TileActor->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);

				int32 x = CurrPiece->GetGridPosition().X;
				int32 y = CurrPiece->GetGridPosition().Y;
				UE_LOG(LogTemp, Error, TEXT("Prima di accedere alla map - Bot"));

				GMode->CriticalSection.Lock();

				// Before moving the piece, set the current tile to be empty
				TileMap[FVector2D(x, y)]->SetTileStatus(PlayerNumber, ETileStatus::EMPTY);

				GMode->CriticalSection.Unlock();
				
				PieceManager->MovePiece(PlayerNumber, SpawnPosition, CurrPiece, Coord);

			} while (!PieceIsPossibleToMove);

		}, 0.5, false);
}

void AChess_RandomPlayer::OnWin()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Wins!"));
	//GameInstance->SetTurnMessage(TEXT("AI Wins!"));
}

void AChess_RandomPlayer::OnLose()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Loses!"));
	//GameInstance->SetTurnMessage(TEXT("AI Loses!"));
}