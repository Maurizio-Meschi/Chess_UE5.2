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

	// AI player has black pieces
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

/*
* @param: none
* @return: none
* @note: Sets a timer and when the time runs out 
*        it randomly chooses the piece to move 
*        and one of his possible legal moves and carries out the move
*/
void AChess_RandomPlayer::OnTurn()
{
	GameInstance->SetTurnMessage(TEXT("AI (Random) Turn"));

	// Get reference to game mode, game field and Piece manager
	AManagePiece* ManagerPiece = nullptr;

	if (!FGameRef::GetManagePiece(this, ManagerPiece, "RandomPlayer"))
		return;

	// Set timer
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

			// Iterate until a possible piece to move has been chosen
			do{
				// Select random piece
				if (PiecesArray.Num() == 0)
				{
					UE_LOG(LogTemp, Error, TEXT("No piece RandomPlayer"));
					return;
				}
				RIndex = FMath::Rand() % PiecesArray.Num();
				AChessPieces* CurrPiece = PiecesArray[RIndex];
				auto CurrPosition = CurrPiece->GetGridPosition();

				// Get the array of legal moves for the chosen piece
				auto TileMarked = ManagerPiece->LegalMoveArray[CurrPiece->IndexArray];

				// Check if piece is possible to move
				if (TileMarked.Num() == 0)
					continue;

				PieceIsPossibleToMove = true;

				// Select the tile to move piece
				int32 RIndexToMovePiece = FMath::Rand() % TileMarked.Num();
				ATile* TileActor = TileMarked[RIndexToMovePiece].Tile;
				bool Capture = TileMarked[RIndexToMovePiece].Capture;

				// Get the coordinates where to move the piece
				FVector2D Coord = TileActor->GetGridPosition();

				TileActor->SetTileStatus(Player::AI, ETileStatus::OCCUPIED);

				// Check if possible to capture an enemy piece
				if (Capture)
				{
					AChessPieces* PieceToCapture = nullptr;
					if (PiecesMap.Contains(Coord))
						PieceToCapture = PiecesMap[(Coord)];

					ManagerPiece->CapturePiece(PieceToCapture, Coord);
				}

				// Before moving the piece, set the current tile empty
				if (TileMap.Contains(CurrPiece->GetGridPosition()))
					TileMap[CurrPiece->GetGridPosition()]->SetTileStatus(-1, ETileStatus::EMPTY);
				
				ManagerPiece->MovePiece(PlayerNumber, CurrPiece, Coord, CurrPosition);

			} while (!PieceIsPossibleToMove);

		}, 2, false);
}

void AChess_RandomPlayer::OnWin()
{
	GameInstance->SetTurnMessage(TEXT("AI Wins!"));
}

void AChess_RandomPlayer::OnLose()
{
	GameInstance->SetTurnMessage(TEXT("AI Loses!"));
}