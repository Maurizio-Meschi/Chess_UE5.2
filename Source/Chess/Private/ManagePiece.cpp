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

void AManagePiece::MovePiece(const int32 PlayerNumber, const FVector& SpawnPosition, AChessPieces* Piece, FVector2D Coord, FVector2D StartPosition)
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
		if (Piece->IsA<AChessPawn>())
		{
			if (TileMap.Contains(Coord) && Capture == "x")
				Capture = TileMap[StartPosition]->Name.Left(1) + "x";
			UE_LOG(LogTemp, Error, TEXT("Capture: %s"), *Capture);
		}
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
		auto PlayerController = Cast<AChess_PlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		if (PlayerController)
			PlayerController->AddInventoryWidgetToViewport();
		else
			UE_LOG(LogTemp, Error, TEXT("Controller null in ManagePiece"));
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
		Obj.Capture = false;
		ArrayOfPlays.Add(Obj);
		
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
	FRewind Obj;
	Obj.PieceToRewind = PieceToCapture;
	Obj.Position = PieceToCapture->GetGridPosition();
	Obj.Capture = true;
	ArrayOfPlays.Add(Obj);

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

void AManagePiece::Replay()
{
	auto PlayerController = Cast<AChess_PlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PlayerController)
		PlayerController->DisableInput(PlayerController);

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

	for (auto Piece : CapturedPieces)
	{
		Piece->SetActorHiddenInGame(true);
	}

	for (auto Piece : PromotePieces)
	{
		UE_LOG(LogTemp, Error, TEXT("Promote piece: %s"), *Piece->GetClass()->GetName());
		Piece->SetActorHiddenInGame(true);
	}

	int32 ArrivalIndex = ButtonValue;
	if (ArrivalIndex > ArrayOfPlays.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Index: ArrivalIndex: %d. ArrayOfPlay Num: %d"), ArrivalIndex, ArrayOfPlays.Num() - 1);
		return;
	}
	
	for (int32 i = 0; i < ArrivalIndex + 1; i++)
	{
		auto Position = ArrayOfPlays[i].Position;
		auto Piece = ArrayOfPlays[i].PieceToRewind;
		if (ArrayOfPlays[i].Capture)
		{
			Piece->SetActorHiddenInGame(true);
			ArrivalIndex++;
		}
		else
			Piece->SetActorHiddenInGame(false);
		FVector NewLocation = Field->GetActorLocation() + FVector(Position.X, Position.Y, 0.0f);
		NewLocation = Field->GetRelativeLocationByXYPosition(NewLocation.X, NewLocation.Y);
		//UE_LOG(LogTemp, Error, TEXT("Position in 2D: %f %f. Position 3D: %f %f %f"), Position.X, Position.Y, FVector(Position, 0).X, FVector(Position, 0).Y, FVector(Position, 0).Z);
		//UE_LOG(LogTemp, Error, TEXT("Position Field: %f %f %f. Position Actor: %f %f %f."), Field->GetActorLocation().X, Field->GetActorLocation().Y, Field->GetActorLocation().Z, NewLocation.X, NewLocation.Y, NewLocation.Z);
		Piece->SetActorLocation(NewLocation);
	}
}

void AManagePiece::BackToPlay()
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

	for (auto Piece : CapturedPieces)
	{
		Piece->SetActorHiddenInGame(true);
	}

	for (auto Piece : PromotePieces)
	{
		UE_LOG(LogTemp, Error, TEXT("Promote piece: %s"), *Piece->GetClass()->GetName());
		Piece->SetActorHiddenInGame(true);
	}

	int32 ArrivalIndex = ButtonValue;
	if (ArrivalIndex > ArrayOfPlays.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Index: ArrivalIndex: %d. ArrayOfPlay Num: %d"), ArrivalIndex, ArrayOfPlays.Num() - 1);
		return;
	}

	for (int32 i = 0; i < ArrayOfPlays.Num(); i++)
	{
		auto Position = ArrayOfPlays[i].Position;
		auto Piece = ArrayOfPlays[i].PieceToRewind;
		if (ArrayOfPlays[i].Capture)
		{
			Piece->SetActorHiddenInGame(true);
			ArrivalIndex++;
		}
		else
			Piece->SetActorHiddenInGame(false);
		FVector NewLocation = Field->GetActorLocation() + FVector(Position.X, Position.Y, 0.0f);
		NewLocation = Field->GetRelativeLocationByXYPosition(NewLocation.X, NewLocation.Y);
		Piece->SetActorLocation(NewLocation);
	}

	auto PlayerController = Cast<AChess_PlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PlayerController)
		PlayerController->EnableInput(PlayerController);
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

	FRewind Obj;
	Obj.PieceToRewind = PieceToPromote;
	Obj.Position = PieceToPromote->GetGridPosition();
	Obj.Capture = true;
	ArrayOfPlays.Add(Obj);

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

	auto PiecesMap = Field->GetPiecesMap();
	if (PiecesMap.Contains(Position))
		PromotePieces.Add(PiecesMap[Position]);

	if (Player == 0)
	{
		auto PlayerController = Cast<AChess_PlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		if (PlayerController)
			PlayerController->RemoveInventoryWidgetToViewport();
		else
			UE_LOG(LogTemp, Error, TEXT("Controller null in ManagePiece"));
	}
	PieceToPromote = nullptr;

	HandlePromotionCompleted();
}

