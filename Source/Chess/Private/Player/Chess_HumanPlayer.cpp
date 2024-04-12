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
	PlayerNumber = 0;
	//PieceColor = EPieceColor::BLACK;
	PieceChoose = false;
	CurrPiece = nullptr;
}

void AChess_HumanPlayer::OnTurn()
{
	MyTurn = true;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Your Turn"));
	GameInstance->SetTurnMessage(TEXT("Human Turn"));
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

/*
* @param: none
* @return: none
* @note: private method for restoring the state of tiles that were marked
*/
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
			Tile->SetTileStatus(Player::AI, ETileStatus::OCCUPIED);
	}
}


/*
* @param: none
* @return: none
* @note: Method that manages the player's click.
*        It takes care of checking that a piece of the human is selected 
*		 and that it is moved to a marked tile
*/
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

			// Clicking on another piece removes the old suggested moves
			Field->TileMarkedDestroy();
			
			// look for the piece to move
			if (FindPiece(ClassName))
			{
				ResetMarkStatus();
				ManageClickPiece(HitActor);
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

/*
* @param: Actor selected by click
* @return: none
* @note: private method that manages the click on a piece of the human. 
*        In particular, it marks the tiles in which the piece can move and 
*        managing the graphics of the possible moves
*/
void AChess_HumanPlayer::ManageClickPiece(AActor* HitActor)
{
	AChess_GameMode* GMode = nullptr;
	AGameField* Field = nullptr;
	AManagePiece* ManagerPiece = nullptr;

	if (!FGameRef::GetGameRef(this, GMode, Field, ManagerPiece, "HumanPlayer"))
		return;

	// Get the piece
	CurrPiece = Cast<AChessPieces>(HitActor);

	// If Chess Piece is black means it is enemy piece
	if (CurrPiece->Color == EPieceColor::BLACK)
		return;

	auto TileMarked = ManagerPiece->LegalMoveArray[CurrPiece->IndexArray];

	// Sets that the player has chosen the piece to move
	PieceChoose = true;

	// if the selected piece cannot move, do nothing
	if (TileMarked.Num() == 0)
		return;

	for (int32 k = 0; k < TileMarked.Num(); k++) {
		if (TileMarked[k].Capture)
			TileMarked[k].Tile->SetTileStatus(Player::Player1, ETileStatus::MARKED_TO_CAPTURE);
		else
			TileMarked[k].Tile->SetTileStatus(Player::Player1, ETileStatus::MARKED);
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
		Field->AddTileMarked(Obj);
	}
}

/*
* @param: 1.) Actor selected by click
*         2.) String corresponding to the class of the clicked object. 
*             The ClassName should be the class of the tile or the class of the piece to capture
* 
* @return: none
* 
* @note: Private method that handles clicking on a tile. 
*        If the tile in which to move the piece is occupied then capture the enemy piece, 
*        otherwise simply move the human's piece
*/
void AChess_HumanPlayer::ManageClickTile(AActor* HitActor, FString ClassName)
{
	AChess_GameMode* GMode = nullptr;
	AGameField* Field = nullptr;
	AManagePiece* ManagerPiece = nullptr;

	if (!FGameRef::GetGameRef(this, GMode, Field, ManagerPiece, "HumanPlayer"))
		return;

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	auto TileMarked = ManagerPiece->LegalMoveArray;

	PieceChoose = false;

	// Handle the case when accessing an empty tile
	if (FindTile(ClassName))
	{
		ATile* TileActor = Cast<ATile>(HitActor);

		if (TileActor->GetTileStatus() == ETileStatus::MARKED)
		{
			TileActor->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);

			FVector2D Coord = TileActor->GetGridPosition();

			// Before moving the piece, set the current tile status to EMPTY
			if (TileMap.Contains(CurrPiece->GetGridPosition()))
				TileMap[CurrPiece->GetGridPosition()]->SetTileStatus(-1, ETileStatus::EMPTY);

			ResetMarkStatus();

			ManagerPiece->MovePiece(PlayerNumber, CurrPiece, Coord, CurrPiece->GetGridPosition());

			MyTurn = false;
		}
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
		{
			EnemyTile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);

			FVector2D Coord = EnemyTile->GetGridPosition();
			auto CurrPosition = CurrPiece->GetGridPosition();

			TMap<FVector2D, AChessPieces*> PiecesMap = Field->GetPiecesMap();

			AChessPieces* PieceToCapture = nullptr;

			// reference to the piece to be captured
			if (PiecesMap.Contains(Coord))
				PieceToCapture = PiecesMap[(Coord)];

			ManagerPiece->CapturePiece(PieceToCapture, Coord);

			// Before moving the piece, set the current tile to be empty
			if (TileMap.Contains(CurrPiece->GetGridPosition()))
				TileMap[CurrPiece->GetGridPosition()]->SetTileStatus(PlayerNumber, ETileStatus::EMPTY);

			ResetMarkStatus();

			ManagerPiece->MovePiece(PlayerNumber, CurrPiece, Coord, CurrPosition);

			MyTurn = false;
		}
	}
}

/*
* @param: String corresponding to the class of the clicked object.
*         The ClassName should be the class of a human piece
*
* @return: True if I clicked on a human piece, false otherwise
*
* @note: none
*/
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

/*
* @param: String corresponding to the class of the clicked object.
*         The ClassName should be the class of the tile
*
* @return: True if I clicked on a chessboard tile, false otherwise
*
* @note: none
*/
bool AChess_HumanPlayer::FindTile(FString ClassName)
{
	for (int32 i = 0; i < 2; i++)
	{
		if (ClassName == TILE_ACTOR_ARRAY[i])
			return true;
	}
	return false;
}

/*
* @param: String corresponding to the class of the clicked object.
*         The ClassName should be the class of a enemy piece
*
* @return: True if I clicked on a enemy piece, false otherwise
*
* @note: none
*/
bool AChess_HumanPlayer::FindPieceToCapture(FString ClassName)
{
	for (int32 i = 0; i < 6; i++)
	{
		if (ClassName == ENEMY_ACTOR[i])
			return true;
	}
	return false;
}


