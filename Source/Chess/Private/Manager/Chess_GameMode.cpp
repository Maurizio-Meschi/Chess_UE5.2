// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/Manager/Chess_GameMode.h"
#include "../Public/Player/Chess_PlayerController.h"
#include "../Public/MainActor/ChessPieces.h"
#include "../Public/Player/Chess_HumanPlayer.h"
#include "../Public/Player/Chess_RandomPlayer.h"
#include "../Public/Manager/Chess_GameInstance.h"
#include "../Public/Player/Chess_Minimax.h"
#include "Async/TaskGraphInterfaces.h"
#include "Async/TaskGraphInterfaces.h"
#include "EngineUtils.h"
#include "Engine/World.h"

AChess_GameMode::AChess_GameMode()
{
	DefaultPawnClass = AChess_HumanPlayer::StaticClass();

	FieldSize = 8;

	MoveCounter = 0;
}

void AChess_GameMode::BeginPlay()
{
	Super::BeginPlay();

	AChess_HumanPlayer* HumanPlayer = Cast<AChess_HumanPlayer>(*TActorIterator<AChess_HumanPlayer>(GetWorld()));

	if (ManagerClass)
	{
		Manager = GetWorld()->SpawnActor<AManagePiece>(ManagerClass);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Manager is null"));

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

	auto GameInstance = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	
	// MiniMax Player
	if (GameInstance->ChooseAiPlayer == "Hard")
	{
		auto* AI = GetWorld()->SpawnActor<AChess_Minimax>(FVector(), FRotator());
		Players.Add(HumanPlayer);
		Players.Add(AI);
	}
	else if (GameInstance->ChooseAiPlayer == "Random-Random")
	{
		auto* Player1 = GetWorld()->SpawnActor<AChess_RandomPlayer>(FVector(), FRotator());
		auto* AI = GetWorld()->SpawnActor<AChess_RandomPlayer>(FVector(), FRotator());

		Players.Add(Player1);
		Players.Add(AI);
	}
	else if (GameInstance->ChooseAiPlayer == "Random-Minimax")
	{
		auto* Player1 = GetWorld()->SpawnActor<AChess_RandomPlayer>(FVector(), FRotator());
		auto* AI = GetWorld()->SpawnActor<AChess_Minimax>(FVector(), FRotator());
		
		Players.Add(Player1);
		Players.Add(AI);
	}
	else if (GameInstance->ChooseAiPlayer == "Minimax-Minimax")
	{
		auto* Player1 = GetWorld()->SpawnActor<AChess_Minimax>(FVector(), FRotator());
		auto* AI = GetWorld()->SpawnActor<AChess_Minimax>(FVector(), FRotator());

		Players.Add(Player1);
		Players.Add(AI);
	}
	else
	{
		auto* AI = GetWorld()->SpawnActor<AChess_RandomPlayer>(FVector(), FRotator());
		Players.Add(HumanPlayer);
		Players.Add(AI);
	}

	this->ChoosePlayerAndStartGame();
}

void AChess_GameMode::BeginDestroy()
{
	Super::BeginDestroy();
	
	FGameRef::ResetCachedGameMode();
}


/*
* @param: none
* @return: none
* @note: assigns the initial turn to the human, 
*        calculates its legal moves and begins the game
*/
void AChess_GameMode::ChoosePlayerAndStartGame()
{
	Manager->IsGameOver = false;

	CurrentPlayer = Player::Player1;

	for (int32 i = 0; i < Players.Num(); i++)
	{
		Players[i]->PlayerNumber = i;
		Players[i]->PieceColor = CurrentPlayer == Player::Player1 ? EPieceColor::WHITE : EPieceColor::BLACK;
	}

	FBoard Board;
	Board.Field = GField->GetTileMap();
	Board.Pieces = GField->GetPiecesMap();


	auto PiecesArray = GField->GetHumanPlayerPieces();

	for (auto Piece : PiecesArray)
		Piece->LegalMove(Board, Player::Player1, false);

	Players[CurrentPlayer]->OnTurn();
}

int32 AChess_GameMode::GetNextPlayer(int32 Player)
{
	Player++;
	MoveCounter++;
	if (!Players.IsValidIndex(Player))
		Player = Player::Player1;
	return Player;
}

void AChess_GameMode::TurnNextPlayer()
{
	CurrentPlayer = GetNextPlayer(CurrentPlayer);
	Players[CurrentPlayer]->OnTurn();
}


/*
* @param: Piece 
* @return: Name of piece
* @note: none
*/
FString AChess_GameMode::Tokenizer(AChessPieces* Piece)
{
	if (Piece->Color == EPieceColor::BLACK)
		return Piece->Name.ToLower();
	else
		return Piece->Name;
}

/*
* @param: reference to chessboard
* @return: FEN string
* @note: Generate a FEN in string to track the chessboard
*/
FString AChess_GameMode::GenerateString(const FBoard& Board)
{
	FString tmp;
	int32 x = 0;
	int32 Cont = 0;
	int32  Normalization = 1;

	auto FieldMap = Board.Field;
	auto PiecesMap = Board.Pieces;

	for (auto Element : FieldMap)
	{
		ATile* Tile = Element.Value;
		auto TilePosition = Tile->GetGridPosition();

		if (PiecesMap.Contains(TilePosition))
		{
			AChessPieces* Piece = PiecesMap[TilePosition];
			if (Cont != 0)
				tmp += (FString::FromInt(Cont) + Tokenizer(Piece));
			else
				tmp += Tokenizer(Piece);
			Cont = 0;
		}
		else
			++Cont;

		x++;

		if (Cont == 8)
		{
			tmp += "8";
			Cont = 0;
		}

		if (x == Normalization * 8)
		{
			if (Cont != 0)
			{
				tmp += (FString::FromInt(Cont) + "/");
				Cont = 0;
			}
			else
				tmp += "/";

			Normalization++;
		}
	}
	return tmp;
}

/*
* @param: reference to chessboard
* @return: true if three identical configurations occurred, false otherwise
* @note: none
*/
bool AChess_GameMode::IsDraw(const FBoard& Board)
{
	FString tmp = GenerateString(Board);

	FEN_Array.Add(tmp);

	int Occurrences = 0;

	for (auto String : FEN_Array)
	{
		if (String == tmp)
		{
			Occurrences++;
		}
	}
	
	return Occurrences >= 3;
}



AChess_GameMode* FGameRef::CachedGameMode = nullptr;

AChess_GameMode* FGameRef::GetGameMode(UObject* WorldContextObject)
{
	if (!CachedGameMode)
	{
		UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
		CachedGameMode = World->GetAuthGameMode<AChess_GameMode>();
	}

	return CachedGameMode;
}

bool FGameRef::GetGameRef(UObject* WorldContextObject, AChess_GameMode*& GMode, AGameField*& Field, AManagePiece*& PieceManager, FString Source)
{
	GMode = FGameRef::GetGameMode(WorldContextObject);
	if (!GMode)
	{
		UE_LOG(LogTemp, Error, TEXT("Game mode null in %s"), *Source);
		return false;
	}
	Field = GMode->GField;
	if (!Field)
	{
		UE_LOG(LogTemp, Error, TEXT("Game Field null in %s"), *Source);
		return false;
	}
	PieceManager = GMode->Manager;
	if (!PieceManager)
	{
		UE_LOG(LogTemp, Error, TEXT("Piece Manager null in %s"), *Source);
		return false;
	}
	return true;
}

bool FGameRef::GetGameField(UObject* WorldContextObject, AGameField*& Field, FString Source)
{
	auto GMode = FGameRef::GetGameMode(WorldContextObject);
	if (GMode)
	{
		Field = GMode->GField;
		if (!Field)
		{
			UE_LOG(LogTemp, Error, TEXT("Game Field null in %s"), *Source);
			return false;
		}
		return true;
	}
	return false;
}

bool FGameRef::GetManagePiece(UObject* WorldContextObject, AManagePiece*& ManagePiece, FString Source)
{
	auto GMode = FGameRef::GetGameMode(WorldContextObject);
	if (GMode)
	{
		ManagePiece = GMode->Manager;
		if (!ManagePiece)
		{
			UE_LOG(LogTemp, Error, TEXT("Game Field null in %s"), *Source);
			return false;
		}
		return true;
	}
	return false;
}

