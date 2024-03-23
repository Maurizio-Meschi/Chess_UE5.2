// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/Player/Chess_RandomPlayer.h"
#include "../Public/MainActor/ChessPieces.h"
#include "../Public/MainActor/GameField.h"
#include "../Public/MainActor/Tile.h"
#include "../Public/Manager/ManagePiece.h"

// Sets default values
AChess_RandomPlayer::AChess_RandomPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GameInstance = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	PieceColor = EPieceColor::BLACK;

	PlayerNumber = 1;
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
	GameInstance->SetTurnMessage(TEXT("AI (Random) Turn"));

	//FTimerHandle TimerHandle;
	AChess_GameMode* GMode = nullptr;
	AGameField* Field = nullptr;
	AManagePiece* ManagerPiece = nullptr;

	if (!FGameRef::GetGameRef(this, GMode, Field, ManagerPiece, "RandomPlayer"))
		return;

	GetWorld()->GetTimerManager().SetTimer(ManagerPiece->TimerHandle, [&]()
		{
			AChess_GameMode* GMode = nullptr;
			AGameField* Field = nullptr;
			AManagePiece* ManagerPiece = nullptr;

			if (!FGameRef::GetGameRef(this, GMode, Field, ManagerPiece, "RandomPlayer"))
				return;

			TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
			TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

			auto PiecesArray = Field->GetBotPieces();

			bool PieceIsPossibleToMove = false;
			int32 RIndex;
			//Field->ResetTileMarked();

			do{
				// select random piece
				if (PiecesArray.Num() == 0)
				{
					UE_LOG(LogTemp, Error, TEXT("No piece RandomPlayer"));
					return;
				}
				RIndex = FMath::Rand() % PiecesArray.Num();
				AChessPieces* CurrPiece = PiecesArray[RIndex];
				auto CurrPosition = CurrPiece->GetGridPosition();

				auto TileMarked = ManagerPiece->LegalMoveArray[CurrPiece->IndexArray];

				if (TileMarked.Num() == 0)
					continue;

				PieceIsPossibleToMove = true;

				// select the marked tile to move
				int32 RIndexToMovePiece = FMath::Rand() % TileMarked.Num();

				// take the tile where move the piece
				ATile* TileActor = TileMarked[RIndexToMovePiece].Tile;
				bool Capture = TileMarked[RIndexToMovePiece].Capture;

				FVector2D Coord = TileActor->GetGridPosition();

				TileActor->SetTileStatus(Player::AI, ETileStatus::OCCUPIED);
				// check if possible to capture an enemy piece
				if (Capture)
				{
					AChessPieces* PieceToCapture = nullptr;
					if (PiecesMap.Contains(Coord))
						PieceToCapture = PiecesMap[(Coord)];

					ManagerPiece->CapturePiece(PieceToCapture, Coord);
				}

				// Before moving the piece, set the current tile to be empty
				if (TileMap.Contains(CurrPiece->GetGridPosition()))
					TileMap[CurrPiece->GetGridPosition()]->SetTileStatus(-1, ETileStatus::EMPTY);
				
				ManagerPiece->MovePiece(PlayerNumber, CurrPiece, Coord, CurrPosition);

			} while (!PieceIsPossibleToMove);

		}, 1.5, false);
}

void AChess_RandomPlayer::OnWin()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Wins!"));
	GameInstance->SetTurnMessage(TEXT("AI Wins!"));
}

void AChess_RandomPlayer::OnLose()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Loses!"));
	GameInstance->SetTurnMessage(TEXT("AI Loses!"));
}