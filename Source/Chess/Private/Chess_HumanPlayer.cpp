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
	PieceToSpawn = nullptr;
	PieceToRemove = nullptr;
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
				bool Element_found = false;
				for (int32 i = 0; i < 2; i++)
				{
					if (ClassName == TileActorArray[i])
					{
						Element_found = true;
						break;
					}
				}
				//Equals(TEXT(TileActor[i]), ESearchCase::IgnoreCase)
				if (Element_found)
				{
					ATile* TileActor = Cast<ATile>(HitActor);
					if (TileActor->GetTileStatus() == ETileStatus::MARKED)
					{
						// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("clicked"));
						TileActor->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);
						FVector SpawnPosition = TileActor->GetActorLocation();
						AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
						if (PieceToSpawn == nullptr || PieceToRemove == nullptr)
							UE_LOG(LogTemp, Error, TEXT("PieceToSpawn or PieceToMove cannot be null"));
						FVector2D Coord = TileActor->GetGridPosition();
						GameMode->MovePiece(PlayerNumber, SpawnPosition, PieceToSpawn, Coord, PieceToRemove);
						MyTurn = false;
						return;
					}
				}
			}
			else
			{
				bool Element_found = false;
				for (int32 i = 0; i < 6; i++)
				{
					if (ClassName == Actor[i])
					{
						Element_found = true;
						break;
					}
				}
				if (Element_found)
				{
					PieceToRemove = Cast<AChessPieces>(HitActor);
					PieceToSpawn = HitActor->GetClass();
					// If Chess Piece is white means it is enemy piece
					if (PieceToRemove->Color == EPieceColor::WHITE)
					{
						return;
					}
					else
					{
						PieceToRemove->ResetTileMarked();
						UE_LOG(LogTemp, Warning, TEXT("Call Legal Move!"));
						PieceToRemove->LegalMove();
						PieceChoose = true;
						if (PieceToRemove->TileMarked.Num() == 0) return;
						for (int32 k = 0; k < PieceToRemove->TileMarked.Num(); k++) {
							// TODO: applicare il materiale che mi rende giocabile la pedina
						}
						return;
						OnClick();
					}
				}
			}
		}

	}
}


