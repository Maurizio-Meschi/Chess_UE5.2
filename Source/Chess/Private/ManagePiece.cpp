// Fill out your copyright notice in the Description page of Project Settings.


#include "ManagePiece.h"
#include "Chess_GameMode.h"
#include "Chess_PlayerController.h"
#include "ChessPieces.h"
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

	Count = 1;

	Capture = "";
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

	GField->PiecesMapRemove(Piece->GetGridPosition());
	Piece->SetGridPosition(Coord.X, Coord.Y);
	
	GField->AddPiecesMap(Coord, Piece);
	Piece->SetActorLocation(NewLocation);

	auto TileMap = GField->GetTileMap();

	ATile* Tile = nullptr;
	if (TileMap.Contains(Coord))
		Tile = TileMap[Coord];
	
	auto GameInstance = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameInstance && Tile)
	{
		GameInstance->SetInfo(FString::FromInt(Count) + TEXT(". ") + Piece->Name + Capture + Tile->Name);
		Count++;
		Capture = "";
		auto PlayerController = Cast<AChess_PlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		if (PlayerController)
			PlayerController->Event.Broadcast();
		else
			UE_LOG(LogTemp, Error, TEXT("Controller null in ManagePiece"));
	}

	//Gestire la grafica che dice lo spostamento della pedina
	if ((Piece->IsA<AChessPawn>()) && Piece->Color == EPieceColor::WHITE && (Piece->GetGridPosition().X == 7.0))
	{
		PawnToPromote = Piece;
		AChess_PlayerController::AddInventoryWidgetToViewport();
	}
	else if ((Piece->IsA<AChessPawn>()) && Piece->Color == EPieceColor::BLACK && (Piece->GetGridPosition().X == 0.0))
	{
		TArray<FString> Class = { "Queen", "Rook", "Bishop", "Knight" };
		int32 RIndex = FMath::Rand() % Class.Num();
		SpawnNewPiece(Piece, Class[RIndex]);
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

	
	GField->PiecesMapRemove(Coord);
	CapturedPieces.Add(PieceToCapture);
	
	if (PieceToCapture->Color == EPieceColor::BLACK)
		GField->BotPiecesRemove(PieceToCapture);
	else
		GField->HumanPlayerPiecesRemove(PieceToCapture);
	

	PieceToCapture->SetActorHiddenInGame(true);
	PieceToCapture->SetActorEnableCollision(false);

	Capture = "x";
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

void AManagePiece::SpawnNewPiece(AChessPieces* PieceToPromote, FString NewPiece)
{
	auto GMode = FGameModeRef::GetGameMode(this);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null in PawnPromotion"));
		return;
	}

	AGameField* Field = GMode->GField;
	if (!Field)
	{
		UE_LOG(LogTemp, Error, TEXT("Field null in PawnPromotion"));
		return;
	}

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	TSubclassOf<AChessPieces> PieceClass;

	auto Position = PieceToPromote->GetGridPosition();
	PieceToPromote->SetActorHiddenInGame(true);
	PieceToPromote->SetActorEnableCollision(false);
	//PieceToPromote->SetActorLocation(FVector(0, 0, 0));
	int32 Player = GMode->CurrentPlayer;
	
	Field->PiecesMapRemove(Position);
	CapturedPieces.Add(PieceToPromote);

	if (PieceToPromote->Color == EPieceColor::BLACK)
		Field->BotPiecesRemove(PieceToPromote);
	else
		Field->HumanPlayerPiecesRemove(PieceToPromote);

	if (NewPiece == "Queen")
	{
		PieceClass = Field->GameFieldSubClass.ChessQueen[Player];
	}
	else if (NewPiece == "Rook")
	{
		PieceClass = Field->GameFieldSubClass.ChessRook[Player];
	}
	else if (NewPiece == "Bishop")
	{
		PieceClass = Field->GameFieldSubClass.ChessBishop[Player];
	}
	else if (NewPiece == "Knight")
	{
		PieceClass = Field->GameFieldSubClass.ChessKnight[Player];
	}

	EPieceColor Color = EPieceColor::WHITE;
	if (Player == 1)
		Color = EPieceColor::BLACK;
	
	Field->GenerateChessPieceInXYPosition(Position.X, Position.Y, PieceClass, Color);

	FRewind Obj;
	Obj.PieceToRewind = PieceToPromote;
	Obj.Position = PieceToPromote->GetGridPosition();
	GMode->ArrayOfPlays.Add(Obj);

	if (Player == 0)
		AChess_PlayerController::RemoveInventoryWidgetToViewport();
	PieceToPromote = nullptr;

	HandlePromotionCompleted();
}

