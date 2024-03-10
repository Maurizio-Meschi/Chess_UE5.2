// Fill out your copyright notice in the Description page of Project Settings.

#include "../Public/Player/Chess_HumanPlayer.h"
#include "../Public/MainActor/ChessPieces.h"
#include "../Public/MainActor/GameField.h"
#include "../Public/MainActor/Tile.h"
#include "../Public/Manager/ManagePiece.h"
#include "../Public/Manager/Chess_GameMode.h"
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

	GameInstance = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

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
	GameInstance->SetTurnMessage(TEXT("Human Turn"));

	AChess_GameMode* GMode = nullptr;
	AGameField* Field = nullptr;
	AManagePiece* PieceManager = nullptr;

	if (!FGameRef::GetGameRef(this, GMode, Field, PieceManager, "ChessPieces"))
		return;
	
	auto PiecesArray = Field->GetHumanPlayerPieces();
	for (auto Piece : PiecesArray)
	{
		Piece->LegalMove(PlayerNumber, false);
	}
}
void AChess_HumanPlayer::OnWin()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("You Win!"));
	GameInstance->SetTurnMessage(TEXT("Human Wins!"));
}

void AChess_HumanPlayer::OnLose()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("You Lose!"));
	GameInstance->SetTurnMessage(TEXT("Human Loses!"));
}

void AChess_HumanPlayer::ResetMarkStatus()
{
	AGameField* Field = nullptr;

	if (!FGameRef::GetGameField(this, Field, "HumanPlayer"))
		return;

	auto TileArray = Field->GetTileMap();
	for (auto Element : TileArray)
	{
		ATile* Tile = Element.Value;
		if (Tile->GetTileStatus() == ETileStatus::MARKED)
			Tile->SetTileStatus(-1, ETileStatus::EMPTY);

		if (Tile->GetTileStatus() == ETileStatus::MARKED_TO_CAPTURE)
			Tile->SetTileStatus(1, ETileStatus::OCCUPIED);
	}
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
			AGameField* Field = nullptr;

			if (!FGameRef::GetGameField(this, Field, "HumanPlayer"))
				return;

			FString ClassName = HitActor->GetClass()->GetName();
			Field->TileMarkedDestroy();
			
			// look for the piece to move
			if (FindPiece(ClassName))
			{
				ResetMarkStatus();
				ManageClickPiece(HitActor, ClassName);
				return;
			}

			// First Hit must be a Chess Piece
			if (PieceChoose)
			{
				ManageClickTile(HitActor, ClassName);
				ResetMarkStatus();
			}
		}

	}
}

void AChess_HumanPlayer::ManageClickPiece(AActor* HitActor, FString ClassName)
{
	AChess_GameMode* GMode = nullptr;
	AGameField* Field = nullptr;
	AManagePiece* ManagerPiece = nullptr;

	if (!FGameRef::GetGameRef(this, GMode, Field, ManagerPiece, "HumanPlayer"))
		return;
	// every time the player clicks on a piece, the reachable tiles are reset
	//Field->ResetTileMarked();

	// get the piece
	CurrPiece = Cast<AChessPieces>(HitActor);

	// If Chess Piece is black means it is enemy piece
	if (CurrPiece->Color == EPieceColor::BLACK)
		return;

	// marks the tiles where the player can move his piece
	//CurrPiece->LegalMove(PlayerNumber, false);

	auto TileMarked = ManagerPiece->TileMarkedForPiece[CurrPiece->IndexArray];

	// sets that the player has chosen the piece to move
	PieceChoose = true;
	UE_LOG(LogTemp, Error, TEXT("Pezzo scelto, ha index = %d e l'array marked ha dim: %d"), CurrPiece->IndexArray, TileMarked.Num());

	// if the selected piece cannot move, do nothing
	if (TileMarked.Num() == 0)
		return;
		

	// spawn marked tile where i can move my piece
	// Togliere gli stati di mark, mettere una bool per l'oggetto e settarlo true se cattuare, false altrimenti
	for (int32 k = 0; k < TileMarked.Num(); k++) {
		if (TileMarked[k].Capture)
			TileMarked[k].Tile->SetTileStatus(0, ETileStatus::MARKED_TO_CAPTURE);
		else
			TileMarked[k].Tile->SetTileStatus(0, ETileStatus::MARKED);
		// get x,y position
		int32 x = TileMarked[k].Tile->GetGridPosition().X;
		int32 y = TileMarked[k].Tile->GetGridPosition().Y;
		FVector Location = Field->GetRelativeLocationByXYPosition(x, y);
		TSubclassOf<ATile> Class = (TileMarked[k].Tile->GetTileStatus() == ETileStatus::MARKED) ? Field->GameFieldSubClass.TileClassMarked : Field->GameFieldSubClass.TileClassPieceToCapture;
		ATile* Obj = GetWorld()->SpawnActor<ATile>(Class, Location, FRotator(0.0f, 0.0f, 0.0f));
		// spawn the marked tile
		const float TileScale = Field->TileSize / 100;
		Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
		Obj->SetGridPosition(x, y);
		Field->AddTileMarkedSpawn(Obj);
	}
}

void AChess_HumanPlayer::ManageClickTile(AActor* HitActor, FString ClassName)
{
	AChess_GameMode* GMode = nullptr;
	AGameField* Field = nullptr;
	AManagePiece* ManagerPiece = nullptr;

	if (!FGameRef::GetGameRef(this, GMode, Field, ManagerPiece, "HumanPlayer"))
		return;

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	auto TileMarked = ManagerPiece->TileMarkedForPiece;

	PieceChoose = false;

	// handle the case when accessing an empty tile
	if (FindTile(ClassName))
	{
		ATile* TileActor = Cast<ATile>(HitActor);

		if (TileActor->GetTileStatus() == ETileStatus::MARKED)
			ManageMovingInEmptyTile(TileActor);
		return;
	}

	// Check if possible to capture an enemy piece
	if (FindPieceToCapture(ClassName))
	{
		AChessPieces* EnemyPiece = Cast<AChessPieces>(HitActor);

		ATile* EnemyTile = nullptr;
		
		if (TileMap.Contains(EnemyPiece->GetGridPosition()))
			EnemyTile = TileMap[(EnemyPiece->GetGridPosition())];
		

		if (EnemyTile && EnemyTile->GetTileStatus() == ETileStatus::MARKED_TO_CAPTURE)
			ManageCaptureInEnemyTile(EnemyTile);
	}
}

void AChess_HumanPlayer::ManageMovingInEmptyTile(ATile* TileActor)
{
	AChess_GameMode* GMode = nullptr;
	AGameField* Field = nullptr;
	AManagePiece* PieceManager = nullptr;

	if (!FGameRef::GetGameRef(this, GMode, Field, PieceManager, "HumanPlayer"))
		return;

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();

	// Set the tile status to OCCUPIED where the piece will be placed
	TileActor->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);

	FVector SpawnPosition = TileActor->GetActorLocation();
	FVector2D Coord = TileActor->GetGridPosition();

	
	// Before moving the piece, set the current tile status to EMPTY
	if (TileMap.Contains(CurrPiece->GetGridPosition()))
		TileMap[CurrPiece->GetGridPosition()]->SetTileStatus(PlayerNumber, ETileStatus::EMPTY);
	
	PieceManager->MovePiece(PlayerNumber, SpawnPosition, CurrPiece, Coord, CurrPiece->GetGridPosition());

	MyTurn = false;
}

void AChess_HumanPlayer::ManageCaptureInEnemyTile(ATile* EnemyTile)
{
	AChess_GameMode* GMode = nullptr;
	AGameField* Field = nullptr;
	AManagePiece* PieceManager = nullptr;

	if (!FGameRef::GetGameRef(this, GMode, Field, PieceManager, "HumanPlayer"))
		return;

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

	// Set the tile status to OCCUPIED where the piece will be placed
	EnemyTile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);

	FVector SpawnPosition = EnemyTile->GetActorLocation();
	FVector2D Coord = EnemyTile->GetGridPosition();
	auto CurrPosition = CurrPiece->GetGridPosition();

	AChessPieces* PieceToCapture = nullptr;
	
	// reference to the piece to be captured
	if (PiecesMap.Contains(Coord))
		PieceToCapture = PiecesMap[(Coord)];
	
	PieceManager->CapturePiece(PieceToCapture, Coord);
	
	// Before moving the piece, set the current tile to be empty
	if (TileMap.Contains(CurrPiece->GetGridPosition()))
		TileMap[CurrPiece->GetGridPosition()]->SetTileStatus(PlayerNumber, ETileStatus::EMPTY);
	
	// move the piece
	PieceManager->MovePiece(PlayerNumber, SpawnPosition, CurrPiece, Coord, CurrPosition);
	
	MyTurn = false;
}

bool AChess_HumanPlayer::FindPiece(FString ClassName)
{
	for (int32 i = 0; i < 6; i++)
	{
		if (ClassName == ACTOR[i])
		{
			return true;
		}
	}
	return false;
}

bool AChess_HumanPlayer::FindTile(FString ClassName)
{
	for (int32 i = 0; i < 2; i++)
	{
		if (ClassName == TILE_ACTOR_ARRAY[i])
			return true;
	}
	return false;
}

bool AChess_HumanPlayer::FindPieceToCapture(FString ClassName)
{
	for (int32 i = 0; i < 6; i++)
	{
		if (ClassName == ENEMY_ACTOR[i])
			return true;
	}
	return false;
}


