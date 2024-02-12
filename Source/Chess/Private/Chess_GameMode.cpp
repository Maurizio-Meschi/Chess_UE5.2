// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_GameMode.h"
#include "Chess_PlayerController.h"
#include "ChessPieces.h"
#include "Chess_HumanPlayer.h"
#include "Chess_RandomPlayer.h"
#include "Chess_GameInstance.h"
#include "PawnPromotion.h"
#include "Async/TaskGraphInterfaces.h"
#include "Async/TaskGraphInterfaces.h"
#include "EngineUtils.h"
#include "Engine/World.h"

AChess_GameMode::AChess_GameMode()
{
	PlayerControllerClass = AChess_PlayerController::StaticClass();

	DefaultPawnClass = AChess_HumanPlayer::StaticClass();

	FieldSize = 8;

	MoveCounter = 0;
}

void AChess_GameMode::BeginPlay()
{
	Super::BeginPlay();

	IsGameOver = false;

	//MoveCounter = 0;

	AChess_HumanPlayer* HumanPlayer = Cast<AChess_HumanPlayer>(*TActorIterator<AChess_HumanPlayer>(GetWorld()));

	if (GameFieldClass != nullptr)
	{
		GField = GetWorld()->SpawnActor<AGameField>(GameFieldClass);
		GField->Size = FieldSize;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Game Field is null"));
	}

	float CameraPosX = ((GField->TileSize * (FieldSize + ((FieldSize - 1) * GField->NormalizedCellPadding) - (FieldSize - 1))) / 2) - (GField->TileSize / 2);
	FVector CameraPos(CameraPosX, CameraPosX, 1000.0f);
	HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());

	Players.Add(HumanPlayer);
	// Random Player
	auto* AI = GetWorld()->SpawnActor<AChess_RandomPlayer>(FVector(), FRotator());

	// MiniMax Player
	//auto* AI = GetWorld()->SpawnActor<ATTT_MinimaxPlayer>(FVector(), FRotator());

	// AI player = 1
	Players.Add(AI);

	PlayerController = Cast<AChess_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	this->ChoosePlayerAndStartGame();
}

void AChess_GameMode::ChoosePlayerAndStartGame()
{
	
	//CurrentPlayer = FMath::RandRange(0, Players.Num() - 1);

	CurrentPlayer = 0;
	for (int32 i = 0; i < Players.Num(); i++)
	{
		Players[i]->PlayerNumber = i;
		Players[i]->PieceColor = i == i == CurrentPlayer ? EPieceColor::WHITE : EPieceColor::BLACK;
	}
	//MoveCounter += 1;
	Players[CurrentPlayer]->OnTurn();
}

void AChess_GameMode::MovePiece(const int32 PlayerNumber, const FVector& SpawnPosition, AChessPieces* Piece, FVector2D Coord)
{
	if (IsGameOver || PlayerNumber != CurrentPlayer)
	{
		return;
	}
	FVector NewLocation = GField->GetActorLocation() + SpawnPosition;

	CriticalSection.Lock();

	GField->PiecesMapRemove(FVector2D(Piece->GetGridPosition().X, Piece->GetGridPosition().Y));
	Piece->SetGridPosition(Coord.X, Coord.Y);
	
	GField->AddPiecesMap(Coord, Piece);
	Piece->SetActorLocation(NewLocation);

	CriticalSection.Unlock();

	if (Piece->GetClass()->GetName() == "BP_w_Pawn_C")
	{
		
		if (static_cast<int32>(Piece->GetGridPosition().X) == 7)
		{
			FGraphEventRef MyTask = FFunctionGraphTask::CreateAndDispatchWhenReady([Piece]()
			{
				// Apri il menu di promozione e metti in pausa il gioco
				//Piece->OpenPromotionMenu(Piece);
				UPawnPromotion* PromotionInstance = NewObject<UPawnPromotion>();
				if (PromotionInstance)
				{
					// Imposta il pezzo da promuovere
					PromotionInstance->SetpieceToPromote(Piece);
					PromotionInstance->IsHumanPlayer = true;
						// Visualizza il menu di promozione
					PromotionInstance->PawnPromotion();

					// Metti in pausa il gioco
					//UGameplayStatics::SetGamePaused(GetWorld(), true);
				}
				PromotionInstance->ConditionalBeginDestroy();
			}, TStatId(), nullptr, ENamedThreads::GameThread);
		}

	}
	// Add the piece reference in the current played 
	UChess_GameInstance* GInstance = Cast<UChess_GameInstance>(GetWorld()->GetAuthGameMode());
	FRewind Obj;
	Obj.PieceToRewind = Piece;  
	Obj.Position = Piece->GetGridPosition();
	ArrayOfPlays.Add(Obj);
	
	CheckWinAndGoNextPlayer(PlayerNumber);
}

void AChess_GameMode::CapturePiece(AChessPieces* PieceToCapture, FVector2D Coord)
{
	CriticalSection.Lock();
	GField->PiecesMapRemove(Coord);

	if (PieceToCapture->Color == EPieceColor::BLACK)
		GField->BotPiecesRemove(PieceToCapture);
	else
		GField->HumanPlayerPiecesRemove(PieceToCapture);

	CriticalSection.Unlock();

	PieceToCapture->SetActorHiddenInGame(true);
	PieceToCapture->SetActorEnableCollision(false);
	//PieceToCapture->PieceDestroy();
}

void AChess_GameMode::CheckWinAndGoNextPlayer(const int32 PlayerNumber)
{
	GField->IsCheckmateSituation = false;
	GField->ResetTileMarked();

	TArray<ATile*>& TileArray = GField->GetTileArray();
	for (int32 i = 0; i < TileArray.Num(); i++)
	{
		TileArray[i]->SetStatusCheckmate(-1, EStatusCheckmate::NEUTRAL);
	}

	bool IsHumanPlayer = static_cast<bool>(CurrentPlayer);
	if (GField->Check(PlayerNumber, IsHumanPlayer))
	{
		GField->IsCheckmateSituation = true;
		GField->ResetTileMarked();
		TArray<AChessPieces*> Pieces = (IsHumanPlayer ? GField->GetHumanPlayerPieces() : GField->GetBotPieces());
		for (int32 i = 0; i < Pieces.Num(); i++)
		{
			Pieces[i]->LegalMove(PlayerNumber, IsHumanPlayer);
			if (GField->GetTileMarked().Num() != 0)
				break;
		}

		if (GField->GetTileMarked().Num() == 0)
		{
			IsGameOver = true;
			Players[CurrentPlayer]->OnWin();
			UE_LOG(LogTemp, Error, TEXT("Vittoria confused uga buga"));
			for (int32 i = 0; i < Players.Num(); i++)
			{
				if (i != CurrentPlayer)
				{
					Players[i]->OnLose();
					return;
				}
			}
		}
	}
	else
		GField->CheckSituation = false;

	GField->ResetTileMarked();
	TurnNextPlayer();
}

int32 AChess_GameMode::GetNextPlayer(int32 Player)
{
	Player++;
	MoveCounter++;
	if (!Players.IsValidIndex(Player))
		Player = 0;
	return Player;
}

void AChess_GameMode::TurnNextPlayer()
{
	//MoveCounter += 1;
	CurrentPlayer = GetNextPlayer(CurrentPlayer);
	Players[CurrentPlayer]->OnTurn();
}