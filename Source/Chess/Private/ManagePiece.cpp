// Fill out your copyright notice in the Description page of Project Settings.


#include "ManagePiece.h"
#include "Chess_GameMode.h"
#include "Chess_PlayerController.h"
#include "ChessPieces.h"
#include "PawnPromotion.h"
#include "Async/TaskGraphInterfaces.h"
#include "Async/TaskGraphInterfaces.h"
#include "EngineUtils.h"
#include "Engine/World.h"

// Sets default values
AManagePiece::AManagePiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	IsGameOver = false;
}

// Called when the game starts or when spawned
void AManagePiece::BeginPlay()
{
	Super::BeginPlay();
}

void AManagePiece::MovePiece(const int32 PlayerNumber, const FVector& SpawnPosition, AChessPieces* Piece, FVector2D Coord)
{
	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null in ManagePiece"));
		return;
	}

	auto GField = GMode->GField;
	if (!GField)
	{
		UE_LOG(LogTemp, Error, TEXT("Field null in ManagePiece"));
		return;
	}

	if (IsGameOver || PlayerNumber != GMode->CurrentPlayer)
	{
		return;
	}

	FVector NewLocation = GField->GetActorLocation() + SpawnPosition;

	GMode->CriticalSection.Lock();
	GField->PiecesMapRemove(Piece->GetGridPosition());
	GMode->CriticalSection.Unlock();

	Piece->SetGridPosition(Coord.X, Coord.Y);
	GMode->CriticalSection.Lock();
	GField->AddPiecesMap(Coord, Piece);
	GMode->CriticalSection.Unlock();
	Piece->SetActorLocation(NewLocation);

	//Gestire la grafica che dice lo spostamento della pedina
	UChess_GameInstance* GameInstance = FGameInstanceRef::GetGameInstance(this);
	if (GameInstance)
	{
		GMode->CriticalSection.Lock();
		GameInstance->SetInfo(GField->GetTileMap()[Coord]->Name);
		GMode->CriticalSection.Unlock();
	}

	bool IsHumanPlayer = !static_cast<bool>(GMode->CurrentPlayer);
	
	if ((Piece->GetClass()->GetName() == "BP_w_Pawn_C" && static_cast<int32>(Piece->GetGridPosition().X) == 7) ||
		Piece->GetClass()->GetName() == "BP_b_Pawn_C" && static_cast<int32>(Piece->GetGridPosition().X) == 0)
	{
		UPawnPromotion* PromotionInstance = UPawnPromotion::GetInstance();
		if (PromotionInstance)
		{
			PromotionInstance->SetpieceToPromote(Piece);
			PromotionInstance->SetManager(this);
			PromotionInstance->IsHumanPlayer = IsHumanPlayer;
			FGraphEventRef GameThreadTask = FFunctionGraphTask::CreateAndDispatchWhenReady([=]()
				{
					PromotionInstance->PawnPromotion();
				}, TStatId(), nullptr, ENamedThreads::GameThread);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("PromotionInstance null in ManagePiece"));
	}
	else
	{
		// Add the piece reference in the current played 
		FRewind Obj;
		Obj.PieceToRewind = Piece;
		Obj.Position = Piece->GetGridPosition();
		GMode->ArrayOfPlays.Add(Obj);

		CheckWinAndGoNextPlayer(PlayerNumber);
	}
}

void AManagePiece::CapturePiece(AChessPieces* PieceToCapture, FVector2D Coord)
{
	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null in ManagePiece"));
		return;
	}

	auto GField = GMode->GField;
	if (!GField)
	{
		UE_LOG(LogTemp, Error, TEXT("Field null in ManagePiece"));
		return;
	}

	GMode->CriticalSection.Lock();
	GField->PiecesMapRemove(Coord);
	
	if (PieceToCapture->Color == EPieceColor::BLACK)
		GField->BotPiecesRemove(PieceToCapture);
	else
		GField->HumanPlayerPiecesRemove(PieceToCapture);
	GMode->CriticalSection.Unlock();

	PieceToCapture->SetActorHiddenInGame(true);
	PieceToCapture->SetActorEnableCollision(false);;
}

void AManagePiece::CheckWinAndGoNextPlayer(const int32 PlayerNumber)
{
	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null in ManagePiece"));
		return;
	}

	auto GField = GMode->GField;
	if (!GField)
	{
		UE_LOG(LogTemp, Error, TEXT("Field null in ManagePiece"));
		return;
	}

	GField->IsCheckmateSituation = false;
	GField->ResetTileMarked();

	TArray<ATile*>& TileArray = GField->GetTileArray();
	for (int32 i = 0; i < TileArray.Num(); i++)
	{
		TileArray[i]->SetStatusCheckmate(-1, EStatusCheckmate::NEUTRAL);
	}

	bool IsHumanPlayer = static_cast<bool>(GMode->CurrentPlayer);

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
			GMode->Players[GMode->CurrentPlayer]->OnWin();

			for (int32 i = 0; i < GMode->Players.Num(); i++)
			{
				if (i != GMode->CurrentPlayer)
				{
					GMode->Players[i]->OnLose();
					return;
				}
			}
		}
	}
	else
		GField->CheckSituation = false;

	GField->ResetTileMarked();
	GMode->TurnNextPlayer();
}

void AManagePiece::HandlePromotionCompleted()
{
	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null in ManagePiece"));
		return;
	}

	int32 CurrPlayer = GMode->CurrentPlayer;
	CheckWinAndGoNextPlayer(CurrPlayer);
}

