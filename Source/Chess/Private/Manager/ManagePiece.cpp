// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/Manager/ManagePiece.h"
#include "../Public/Manager/Chess_GameMode.h"
#include "../Public/Player/Chess_PlayerController.h"
#include "../Public/MainActor/ChessPieces.h"
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
	AChess_GameMode* GMode = FGameRef::GetGameMode(this);
	AGameField* GField = nullptr;
	
	if (!GMode || !FGameRef::GetGameField(this, GField, "ManagePiece"))
		return;
	
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
		}
		GameInstance->SetInfo(FString::FromInt(Count) + TEXT(". ") + Piece->Name + Capture + Tile->Name);
		//Count++;
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
	AGameField* GField = nullptr;

	if (!FGameRef::GetGameField(this, GField, "ManagePiece"))
		return;

	
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
	AChess_GameMode* GMode = FGameRef::GetGameMode(this);
	AGameField* Field = nullptr;

	if (!GMode || !FGameRef::GetGameField(this, Field, "ManagePiece"))
		return;
	
	for (int32 i = 0; i < TileMarkedForPiece.Num(); i++)
		TileMarkedForPiece[i].Empty();

	// Prima di andare al prossimo turno devo vedere se il prossimo giocatore ha mosse disponibili
	auto PiecesArray = GMode->CurrentPlayer == 1 ? Field->GetHumanPlayerPieces() : Field->GetBotPieces();

	for (auto Piece : PiecesArray)
		Piece->LegalMove(GMode->CurrentPlayer == 0? 1:0, false);

	int32 Cont = 0;
	for (int32 i = 0; i < TileMarkedForPiece.Num(); i++)
	{
		if (TileMarkedForPiece[i].Num() > 0)
			break;
		else
			Cont++;
	}

	UE_LOG(LogTemp, Error, TEXT("TileMarked ha dim %d, Cont ha dim %d"), TileMarkedForPiece.Num(), Cont);
	if (Cont == TileMarkedForPiece.Num())
	{
		IsGameOver = true;
		GMode->Players[GMode->CurrentPlayer]->OnWin();
		return;
	}

	for (int32 i = 0; i < TileMarkedForPiece.Num(); i++)
		TileMarkedForPiece[i].Empty();

	if (GMode->CurrentPlayer == 1)
	{
		Visible = true;
		IsBotPlayed = true;
		Count++;
	}
	else
	{
		Visible = false;
		IsBotPlayed = false;
	}
	DisableButtonEvent.Broadcast();
	GMode->TurnNextPlayer();
}

void AManagePiece::RewindManager(int32 MoveNumber)
{
	AGameField* Field = nullptr;

	if (!FGameRef::GetGameField(this, Field, "ManagePiece"))
		return;

	Field->TileMarkedDestroy();

	for (auto Piece : CapturedPieces)
	{
		Piece->SetActorHiddenInGame(true);
	}

	for (auto Piece : PromotePieces)
	{
		Piece->SetActorHiddenInGame(true);
	}

	if (MoveNumber > ArrayOfPlays.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Index"));
		return;
	}

	for (int32 i = 0; i < MoveNumber; i++)
	{
		auto Position = ArrayOfPlays[i].Position;
		auto Piece = ArrayOfPlays[i].PieceToRewind;
		if (ArrayOfPlays[i].Capture)
		{
			Piece->SetActorHiddenInGame(true);
			if (MoveNumber < ArrayOfPlays.Num())
				MoveNumber++;
		}
		else
			Piece->SetActorHiddenInGame(false);
		FVector NewLocation = Field->GetActorLocation() + FVector(Position.X, Position.Y, 0.0f);
		NewLocation = Field->GetRelativeLocationByXYPosition(NewLocation.X, NewLocation.Y);
		Piece->SetActorLocation(NewLocation);
	}
}

void AManagePiece::Replay()
{
	auto PlayerController = Cast<AChess_PlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PlayerController)
		PlayerController->DisableInput(PlayerController);

	RewindManager(ButtonValue + 1);
}

void AManagePiece::BackToPlay()
{
	RewindManager(ArrayOfPlays.Num());

	auto PlayerController = Cast<AChess_PlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PlayerController)
		PlayerController->EnableInput(PlayerController);
}

void AManagePiece::HandlePromotionCompleted()
{
	auto GMode = FGameRef::GetGameMode(this);
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
	AChess_GameMode* GMode = FGameRef::GetGameMode(this);
	AGameField* Field = nullptr;

	if (!GMode || !FGameRef::GetGameField(this, Field, "ManagePiece"))
		return;

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

