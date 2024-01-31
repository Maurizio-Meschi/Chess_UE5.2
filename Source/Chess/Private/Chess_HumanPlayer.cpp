// Fill out your copyright notice in the Description page of Project Settings.

#include "Chess_HumanPlayer.h"
#include "ChessPieces.h"
#include "GameField.h"
#include "Tile.h"
#include "Chess_GameMode.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Called when the game starts or when spawned
void AChess_HumanPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChess_HumanPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AChess_HumanPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

AChess_HumanPlayer::AChess_HumanPlayer() 
{
	PrimaryActorTick.bCanEverTick = true;
	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	// create a camera component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	//set the camera as RootComponent
	SetRootComponent(Camera);

	//GameInstance = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	//default value
	PlayerNumber = -1;
	PieceColor = EPieceColor::BLACK;
	PieceChoose = false;
	CurrPiece = nullptr;
}

void AChess_HumanPlayer::OnTurn()
{
	MyTurn = true;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Your Turn"));
	UE_LOG(LogTemp, Warning, TEXT("Turno mio!"));
	//GameInstance->SetTurnMessage(TEXT("Human Turn"));
}
void AChess_HumanPlayer::OnWin()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("You Win!"));
	//GameInstance->SetTurnMessage(TEXT("Human Wins!"));
	//GameInstance->IncrementScoreHumanPlayer();
}

void AChess_HumanPlayer::OnLose()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("You Lose!"));
	//GameInstance->SetTurnMessage(TEXT("Human Loses!"));
}

void AChess_HumanPlayer::OnClick()
{
	//Structure containing information about one hit of a trace, such as point of impact and surface normal at that point
	FHitResult Hit = FHitResult(ForceInit);
	// GetHitResultUnderCursor function sends a ray from the mouse position and gives the corresponding hit results
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);
	if (Hit.bBlockingHit && MyTurn)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			FString ClassName = HitActor->GetClass()->GetName();
			// First Hit must be a Chess Piece
			if (PieceChoose)
			{
				PieceChoose = false;
				//Equals(TEXT(TileActor[i]), ESearchCase::IgnoreCase)
				if (FindTile(ClassName))
				{
					ATile* TileActor = Cast<ATile>(HitActor);
					if (TileActor->GetTileStatus() == ETileStatus::MARKED)
					{
						TileActor->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);
						FVector SpawnPosition = TileActor->GetActorLocation();
						AChess_GameMode* GMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
						FVector2D Coord = TileActor->GetGridPosition();

						// Before moving the piece, set the current tile to be empty
						GMode->GField->TileMap[FVector2D(CurrPiece->GetGridPosition()[0], CurrPiece->GetGridPosition()[1])]->SetTileStatus(PlayerNumber, ETileStatus::EMPTY);
						
						GMode->MovePiece(PlayerNumber, SpawnPosition, CurrPiece, Coord);
						MyTurn = false;
						return;
					}
				}
				// Check if possible to capture an enemy piece
				if (FindPieceToCapture(ClassName))
				{
					AChess_GameMode* GMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
					AChessPieces* EnemyPiece = Cast<AChessPieces>(HitActor);
					ATile* EnemyTile = GMode->GField->TileMap[(EnemyPiece->GetGridPosition())];

					if (EnemyTile->GetTileStatus() == ETileStatus::MARKED_TO_CAPTURE) {
						EnemyTile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);
						FVector SpawnPosition = EnemyTile->GetActorLocation();
						FVector2D Coord = EnemyTile->GetGridPosition();

						AChessPieces* PieceToCapture = GMode->GField->PiecesMap[(Coord)];
						GMode->CapturePiece(PieceToCapture, Coord);

						// Before moving the piece, set the current tile to be empty
						GMode->GField->TileMap[FVector2D(CurrPiece->GetGridPosition()[0], CurrPiece->GetGridPosition()[1])]->SetTileStatus(PlayerNumber, ETileStatus::EMPTY);

						GMode->MovePiece(PlayerNumber, SpawnPosition, CurrPiece, Coord);
						MyTurn = false;
						return;
					}
				}
			}
			if (FindPiece(ClassName))
			{
				CurrPiece = Cast<AChessPieces>(HitActor);
				// If Chess Piece is white means it is enemy piece
				if (CurrPiece->Color == EPieceColor::WHITE)
				{
					return;
				}
				else
				{
					AChess_GameMode* GMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
					TArray<ATile*> CurrTileArray = GMode->GField->TileArray;
					for (int32 i = 0; i < CurrTileArray.Num(); i++)
					{
						if (CurrTileArray[i]->GetTileStatus() == ETileStatus::MARKED)
							CurrTileArray[i]->SetTileStatus(-1, ETileStatus::EMPTY);

						if (CurrTileArray[i]->GetTileStatus() == ETileStatus::MARKED_TO_CAPTURE)
							CurrTileArray[i]->SetTileStatus(1, ETileStatus::OCCUPIED);
					}
					CurrPiece->ResetTileMarked();
					CurrPiece->LegalMove(PlayerNumber, true);
					PieceChoose = true;
					if (CurrPiece->TileMarked.Num() == 0) return;
					for (int32 k = 0; k < CurrPiece->TileMarked.Num(); k++) {
						// TODO: applicare il materiale che mi rende giocabile la pedina
					}
				}
			}
		}

	}
}

bool AChess_HumanPlayer::FindPiece(FString ClassName)
{
	for (int32 i = 0; i < 6; i++)
	{
		if (ClassName == Actor[i])
			return true;
	}
	return false;
}

bool AChess_HumanPlayer::FindTile(FString ClassName)
{
	for (int32 i = 0; i < 2; i++)
	{
		if (ClassName == TileActorArray[i])
			return true;
	}
	return false;
}

bool AChess_HumanPlayer::FindPieceToCapture(FString ClassName)
{
	for (int32 i = 0; i < 6; i++)
	{
		if (ClassName == EnemyActor[i])
			return true;
	}
	return false;
}


