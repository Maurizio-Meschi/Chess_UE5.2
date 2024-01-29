// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_RandomPlayer.h"

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
	/*
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Turn"));
	//GameInstance->SetTurnMessage(TEXT("AI (Random) Turn"));

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			TArray<ATile*> FreeCells;
			AChess_GameMode* GameMode = (AChess_GameMode*)(GetWorld()->GetAuthGameMode());
			for (auto& CurrTile : GameMode->GField->GetTileArray())
			{
				if (CurrTile->GetTileStatus() == ETileStatus::EMPTY)
				{
					FreeCells.Add(CurrTile);
				}
			}

			if (FreeCells.Num() > 0)
			{
				int32 RandIdx = FMath::Rand() % FreeCells.Num();
				FVector Location = GameMode->GField->GetRelativeLocationByXYPosition((FreeCells[RandIdx])->GetGridPosition()[0], (FreeCells[RandIdx])->GetGridPosition()[1]);
				FreeCells[RandIdx]->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);

				//GameMode->SetCellSign(PlayerNumber, Location);

			}
		}, 3, false);
		*/
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