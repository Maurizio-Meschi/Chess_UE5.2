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
	PieceColor = EPieceColor::BLACK;
}

// Called when the game starts or when spawned
void AChess_RandomPlayer::BeginPlay()
{
	Super::BeginPlay();
}

void AChess_RandomPlayer::BeginDestroy()
{
	Super::BeginDestroy();

	GameInstance = nullptr;
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
	GameInstance->SetTurnMessage(TEXT("AI (Random) Turn"));
	
	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			auto GMode = FGameModeRef::GetGameMode(this);
			if (!GMode)
			{
				UE_LOG(LogTemp, Error, TEXT("Game mode null RandomPlayer"));
				return;
			}

			AGameField* Field = GMode->GField;
			if (!Field)
			{
				UE_LOG(LogTemp, Error, TEXT("Field null RandomPlayer"));
				return;
			}

			auto PieceManager = GMode->Manager;
			if (!PieceManager)
			{
				UE_LOG(LogTemp, Error, TEXT("PieceManager null RandomPlayer"));
				return;
			}

			TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
			TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

			TArray<AChessPieces*> PiecesArray = Field->GetBotPieces();

			bool PieceIsPossibleToMove = false;
			int32 RIndex;
			Field->ResetTileMarked();

			do{
				// select random piece
				if (PiecesArray.Num() == 0)
				{
					UE_LOG(LogTemp, Error, TEXT("No piece RandomPlayer"));
					return;
				}
				RIndex = FMath::Rand() % PiecesArray.Num();
				AChessPieces* CurrPiece = PiecesArray[RIndex];
				// check the possible move
				CurrPiece->LegalMove(PlayerNumber, false);

				TArray<ATile*> TileMarked = Field->GetTileMarked();

				if (TileMarked.Num() == 0)
					continue;

				PieceIsPossibleToMove = true;

				if (TileMarked.Num() == 0)
				{
					UE_LOG(LogTemp, Error, TEXT("No tile marked RandomPlayer"));
					return;
				}
				// select the marked tile to move
				int32 RIndexToMovePiece = FMath::Rand() % TileMarked.Num();

				// take the tile where move the piece
				ATile* TileActor = TileMarked[RIndexToMovePiece];
				FVector SpawnPosition = TileActor->GetActorLocation();

				FVector2D Coord = TileActor->GetGridPosition();

				// check if possible to capture an enemy piece
				if (TileActor->GetTileStatus() == ETileStatus::MARKED_TO_CAPTURE)
				{
					TileActor->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);

					

					AChessPieces* PieceToCapture = nullptr;
					if (PiecesMap.Contains(Coord))
						PieceToCapture = PiecesMap[(Coord)];

					

					PieceManager->CapturePiece(PieceToCapture, Coord);
				}
				TileActor->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);

				int32 x = CurrPiece->GetGridPosition().X;
				int32 y = CurrPiece->GetGridPosition().Y;
				//UE_LOG(LogTemp, Error, TEXT("Prima di accedere alla map - Bot"));

				
				// Before moving the piece, set the current tile to be empty
				if (TileMap.Contains(FVector2D(x, y)))
					TileMap[FVector2D(x, y)]->SetTileStatus(PlayerNumber, ETileStatus::EMPTY);
				
				
				PieceManager->MovePiece(PlayerNumber, SpawnPosition, CurrPiece, Coord);

			} while (!PieceIsPossibleToMove);

		}, 2, false);
}

void AChess_RandomPlayer::OnWin()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Wins!"));
	GameInstance->SetTurnMessage(TEXT("AI Wins!"));
}

void AChess_RandomPlayer::OnLose()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Loses!"));
	GameInstance->SetTurnMessage(TEXT("AI Loses!"));
}