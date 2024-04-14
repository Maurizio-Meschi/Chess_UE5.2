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
	Visible = true;
	IsBotPlayed = true;

	MoveCounter = 1;
	ButtonValue = 0;

	Capture = "";
}

bool AManagePiece::Castling = false;

// Called when the game starts or when spawned
void AManagePiece::BeginPlay()
{
	Super::BeginPlay();
}


/*
* @param: 1.) Reference to the chessboard
*         2.) Player whose legal moves you want to calculate
*
* @return: two-dimensional array of legal moves
*
* @note: none
*/
TArray<TArray<FMarked>>& AManagePiece::GetAllLegalMoveByPlayer(FBoard& Board, int8 Player)
{
	AGameField* GField = nullptr;

	if (FGameRef::GetGameField(this, GField, "ManagePiece"))
	{
		auto PiecesArray = (Player == Player::Player1 ? GField->GetPlayer1Pieces() : GField->GetAIPieces());

		for (auto Piece : PiecesArray)
		{
			if (!Board.CapturedPieces.Contains(Piece))
				Piece->LegalMove(Board, Player, false);
		}
	}

	return LegalMoveArray;
}


/*
* @param: 1.) Reference to the chessboard
*         2.) Player to whom the piece belongs
*         3.) Piece on which to calculate the legal moves
*
* @return: array of legal moves of the piece
*
* @note: none
*/
TArray<FMarked>& AManagePiece::GetLegalMoveByPiece(FBoard& Board, int32 Player, AChessPieces* Piece)
{
	LegalMoveArray[Piece->IndexArray].Empty();

	Piece->LegalMove(Board, Player, false);

	return LegalMoveArray[Piece->IndexArray];
}


void AManagePiece::ResetLegalMoveArray()
{
	for (int32 i = 0; i < LegalMoveArray.Num(); i++)
		LegalMoveArray[i].Empty();
}


/*
* @param: none
* @return: true if is checkmate, false otherwise
* @note: calculates the legal moves of the opposing player 
*        and if there are none available it is checkmate
*/
bool AManagePiece::IsCheckMate()
{
	AChess_GameMode* GMode = FGameRef::GetGameMode(this);
	AGameField* Field = nullptr;

	if (!GMode || !FGameRef::GetGameField(this, Field, "ManagePiece"))
		return false;

	ResetLegalMoveArray();

	FBoard Board;
	Board.Field = Field->GetTileMap();
	Board.Pieces = Field->GetPiecesMap();


	auto TileMarked = GetAllLegalMoveByPlayer(Board, GMode->CurrentPlayer == Player::AI ? Player::Player1 : Player::AI);

	int32 Cont = 0;
	for (int32 i = 0; i < TileMarked.Num(); i++)
	{
		if (TileMarked[i].Num() > 0)
			break;
		else
			Cont++;
	}

	if (Cont == TileMarked.Num())
		return true;

	return false;
}

/*
* @param: 1.) Player
*         2.) Piece to move
*         3.) Position to move the piece to
*         4.) Position where the piece was located
*
* @return: none
*
* @note: The method deals with moving the piece by managing:
*           1.) Castling
*			2.) Nomenclature
*           3.) The promotion of the pawn
*/
void AManagePiece::MovePiece(const int32 PlayerNumber, AChessPieces* Piece, FVector2D Coord, FVector2D StartPosition)
{
	AChess_GameMode* GMode = FGameRef::GetGameMode(this);
	AGameField* GField = nullptr;
	
	if (!GMode || !FGameRef::GetGameField(this, GField, "ManagePiece"))
		return;
	
	if (IsGameOver || PlayerNumber != GMode->CurrentPlayer)
	{
		return;
	}

	// 1.) Castling

	bool IsCastling = false;
	auto LeftRookPos = PlayerNumber == Player::Player1 ? HR1_POSITION : AIR1_POSITION;
	FVector2D CastlingTilePos(LeftRookPos.X, LeftRookPos.Y + 2);
	
	// Long castling
	if (Piece->IsA<AKing>() && AManagePiece::Castling && Coord == CastlingTilePos)
	{
		CastlingTilePos.Y += 1;
		CastlingManager(LeftRookPos, CastlingTilePos, Coord, Piece);
		IsCastling = true;
	}

	auto RightRookPos = PlayerNumber == Player::Player1 ? HR2_POSITION : AIR2_POSITION;
	CastlingTilePos.X = RightRookPos.X;
	CastlingTilePos.Y = RightRookPos.Y - 1;

	// Castling
	if (Piece->IsA<AKing>() && AManagePiece::Castling && Coord == CastlingTilePos)
	{
		CastlingTilePos.Y -= 1;
		CastlingManager(RightRookPos, CastlingTilePos, Coord, Piece);
		IsCastling = true;
	}

	// Std case
	if (!IsCastling)
	{
		if (Piece->IsA<AKing>() && Cast<AKing>(Piece)->NeverMoved)
			Cast<AKing>(Piece)->NeverMoved = false;

		if (Piece->IsA<ARook>() && Cast<ARook>(Piece)->NeverMoved)
			Cast<ARook>(Piece)->NeverMoved = false;

		FVector NewLocation = GField->GetRelativeLocationByXYPosition(Coord.X, Coord.Y);

		// Update data structures and move the piece
		GField->PiecesMapRemove(Piece->GetGridPosition());
		Piece->SetGridPosition(Coord.X, Coord.Y);

		GField->AddPiecesMap(Coord, Piece);
		Piece->SetActorLocation(NewLocation);
	}

	if (AManagePiece::Castling)
		AManagePiece::Castling = false;

	// 2.) Nomenclature

	auto TileMap = GField->GetTileMap();

	// Get the tile where the piece moves to get name information
	ATile* Tile = nullptr;
	if (TileMap.Contains(Coord))
		Tile = TileMap[Coord];
	
	auto GameInstance = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GameInstance && Tile)
	{
		// Manage pawn capture nomenclature
		if (Piece->IsA<AChessPawn>())
		{
			if (TileMap.Contains(Coord) && Capture == "x")
				Capture = TileMap[StartPosition]->Name.Left(1) + "x";
		}

		// Manage check/checkmate nomenclature
		FBoard Board;
		Board.Field = GField->GetTileMap();
		Board.Pieces = GField->GetPiecesMap();
		FString CheckNotation;

		if (IsCheckMate())
			CheckNotation = "#";

		else if (Piece->LegalMove(Board, GMode->CurrentPlayer, true))
			CheckNotation = "+";
		else 
			CheckNotation = "";
		
		FString PieceName = "";
		if (!Piece->IsA<AChessPawn>())
			PieceName = Piece->Name;

		GameInstance->SetInfo(FString::FromInt(MoveCounter) + TEXT(". ") + PieceName + Capture + Tile->Name + CheckNotation);
		
		Capture = "";
		auto PlayerController = Cast<AChess_PlayerController>(UGameplayStatics::GetPlayerController(this, 0));

		// Broadcast event for the button spawn with the set text
		if (PlayerController)
			PlayerController->SpawnButtonEvent.Broadcast();
		else
			UE_LOG(LogTemp, Error, TEXT("Controller null in ManagePiece"));
	}

	// 3.) Promotion

	// Manage pawn promotion for human player
	
	if ((Piece->IsA<AChessPawn>()) && Piece->Color == EPieceColor::WHITE && (Piece->GetGridPosition().X == 7.0)
		&& (GameInstance->ChooseAiPlayer == "Hard" || GameInstance->ChooseAiPlayer == "Easy"))
	{
		PawnToPromote = Piece;
		auto PlayerController = Cast<AChess_PlayerController>(UGameplayStatics::GetPlayerController(this, 0));

		if (PlayerController)
			PlayerController->AddWidgetPawnPromotionToViewport();
		else
			UE_LOG(LogTemp, Error, TEXT("Controller null in ManagePiece"));
	}
	// Manage pawn promotion for AI player
	else if ((Piece->IsA<AChessPawn>()) && Piece->Color == EPieceColor::BLACK && (Piece->GetGridPosition().X == 0.0))
	{
		if (GameInstance->ChooseAiPlayer == "Easy" ||
			GameInstance->ChooseAiPlayer == "Random-Random")
		{
			TArray<FString> Class = { "Queen", "Rook", "Bishop", "Knight" };
			int32 RIndex = FMath::Rand() % Class.Num();
			SpawnNewPiece(Piece, Class[RIndex]);
		}
		else 
			SpawnNewPiece(Piece, "Queen");
	}
	// Manage pawn promotion for second AI player
	else if ((Piece->IsA<AChessPawn>()) && Piece->Color == EPieceColor::WHITE && (Piece->GetGridPosition().X == 7.0))
	{
		if (GameInstance->ChooseAiPlayer == "Hard" || GameInstance->ChooseAiPlayer == "Minimax-Minimax")
			SpawnNewPiece(Piece, "Queen");
		else
		{ 
			TArray<FString> Class = { "Queen", "Rook", "Bishop", "Knight" };
			int32 RIndex = FMath::Rand() % Class.Num();
			SpawnNewPiece(Piece, Class[RIndex]);
		}
	}
	else 
	{
		// Save the move to the array
		FRewind Obj;
		Obj.PieceToRewind = Piece;
		Obj.Position = Piece->GetGridPosition();
		Obj.Capture = false;

		ArrayOfPlays.Add(Obj);

		CheckWinAndGoNextPlayer();
	}
}


/*
* @param: 1.) Start rook position
*         2.) New rook position
*         3.) New King position
*         4.) Pointer to the king
*
* @return: none
*
* @note: moves king and rook according to the rules of castling
*/
void AManagePiece::CastlingManager(FVector2D StartRookCoord, FVector2d NewRookCoord, FVector2D Coord, AChessPieces* Piece)
{
	AChess_GameMode* GMode = FGameRef::GetGameMode(this);
	AGameField* GField = nullptr;

	if (!GMode || !FGameRef::GetGameField(this, GField, "ManagePiece"))
		return;

	auto PiecesMap = GField->GetPiecesMap();
	auto TileMap = GField->GetTileMap();

	AChessPieces* Rook = nullptr;
	if (PiecesMap.Contains(StartRookCoord))
	{
		Rook = PiecesMap[StartRookCoord];
		if (Rook->IsA<ARook>() && Cast<ARook>(Rook)->NeverMoved)
		{
			// Set the tile status where the rook was located empty
			if (TileMap.Contains(StartRookCoord))
				TileMap[StartRookCoord]->SetTileStatus(-1, ETileStatus::EMPTY);

			// Set the tile status where the rook moves occupied
			if (TileMap.Contains(NewRookCoord))
				TileMap[NewRookCoord]->SetTileStatus(GMode->CurrentPlayer, ETileStatus::OCCUPIED);

			// Update data structures and move the piece
			GField->PiecesMapRemove(Piece->GetGridPosition());
			GField->PiecesMapRemove(Rook->GetGridPosition());

			Piece->SetGridPosition(Coord.X, Coord.Y);
			Rook->SetGridPosition(NewRookCoord.X, NewRookCoord.Y);

			GField->AddPiecesMap(Coord, Piece);
			GField->AddPiecesMap(NewRookCoord, Rook);

			auto RookPosition = GField->GetRelativeLocationByXYPosition(NewRookCoord.X, NewRookCoord.Y);
			auto PiecePosition = GField->GetRelativeLocationByXYPosition(Coord.X, Coord.Y);

			Piece->SetActorLocation(PiecePosition);
			Rook->SetActorLocation(RookPosition);

			// Save the move to the array
			FRewind Obj;
			Obj.PieceToRewind = Rook;
			Obj.Position = Rook->GetGridPosition();
			Obj.Capture = false;
			Obj.Castling = true;

			ArrayOfPlays.Add(Obj);
		}
	}
}


/*
* @param: 1.) Pointer to the piece to capture
*         2.) Position of the piece to capture
*
* @return: none
*
* @note: none
*/
void AManagePiece::CapturePiece(AChessPieces* PieceToCapture, FVector2D Coord)
{
	AGameField* GField = nullptr;

	if (!FGameRef::GetGameField(this, GField, "ManagePiece"))
		return;

	// Update data structures
	GField->PiecesMapRemove(Coord);
	CapturedPieces.Add(PieceToCapture);
	
	if (PieceToCapture->Color == EPieceColor::BLACK)
		GField->AIPiecesRemove(PieceToCapture);
	else
		GField->Player1PiecesRemove(PieceToCapture);
	
	PieceToCapture->SetActorHiddenInGame(true);
	PieceToCapture->SetActorEnableCollision(false);

	// The actor is not destroyed to ensure the replay works correctly
	FRewind Obj;
	Obj.PieceToRewind = PieceToCapture;
	Obj.Position = PieceToCapture->GetGridPosition();
	Obj.Capture = true;

	ArrayOfPlays.Add(Obj);

	Capture = "x";
}


/*
* @param: none
* @return: none
* @note: destroys all references from arrays and cleans it
*/
void AManagePiece::ResetData()
{
	for (int32 i = 0; i < CapturedPieces.Num(); i++)
	{
		CapturedPieces[i]->Destroy();
	}
	CapturedPieces.Empty();

	for (int32 i = 0; i < PromotePieces.Num(); i++)
	{
		PromotePieces[i]->Destroy();
	}
	PromotePieces.Empty();

	for (int32 i = 0; i < ArrayOfPlays.Num(); i++)
	{
		ArrayOfPlays[i].PieceToRewind->Destroy();
	}
	ArrayOfPlays.Empty();

	for (int32 i = 0; i < LegalMoveArray.Num(); i++)
		LegalMoveArray[i].Empty();
	LegalMoveArray.Empty();

	MoveCounter = 1;

	Visible = true;
	IsBotPlayed = true;
}


/*
* @param: none
* @return: none
* @note: remove the timer if set to avoid crashes when resetting or quitting the game
*/
void AManagePiece::DeleteTime()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle);
	}
}

/*
* @param: none
* @return: none
* @note: handles the button disabling event during the AI's turn, 
*        controls checkmate and draw, and moves on to the next player's turn.
*		 At the end of the game the result is written to a file
*/
void AManagePiece::CheckWinAndGoNextPlayer()
{
	AChess_GameMode* GMode = FGameRef::GetGameMode(this);
	AGameField* Field = nullptr;

	if (!GMode || !FGameRef::GetGameField(this, Field, "ManagePiece"))
		return;
	auto GameInstance = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	FString FileName;

	if (GameInstance->ChooseAiPlayer == "Easy")
		FileName = "Human-Random";
	else if (GameInstance->ChooseAiPlayer == "Hard")
		FileName = "Human-Minimax";
	else
		FileName = GameInstance->ChooseAiPlayer;

	FString CSVFilePath = FPaths::ProjectDir() + "Game_Data/CSV/" + FileName + ".csv";

	FString ExistingContent;
	
	// Operations to define whether to enable or disable the button
	Visible = false;

	if (GameInstance->ChooseAiPlayer != "Hard" && GameInstance->ChooseAiPlayer != "Easy")
		Visible = true;
	
	if (GMode->CurrentPlayer == Player::AI)
	{
		MoveCounter++;
		IsBotPlayed = true;
	}
	else
		IsBotPlayed = false;

	if (GMode->CurrentPlayer != Player::AI &&
			(GameInstance->ChooseAiPlayer == "Hard" || GameInstance->ChooseAiPlayer == "Easy"))
	{
		Visible = true;
	}
	
	DisableButtonEvent.Broadcast();

	FBoard Board;
	Board.Field = Field->GetTileMap();
	Board.Pieces = Field->GetPiecesMap();

	// Draw
	if (Field->GetPiecesMap().Num() == 2 || GMode->IsDraw(Board))
	{
		IsGameOver = true;
		IsBotPlayed = true;
		Visible = false;

		DisableButtonEvent.Broadcast();

		GMode->Players[GMode->CurrentPlayer]->OnDraw();

		// Add match information to the file
		FFileHelper::LoadFileToString(ExistingContent, *CSVFilePath);

		TArray<FString> FileLines;
		ExistingContent.ParseIntoArrayLines(FileLines);

		ExistingContent += FString::Printf(TEXT("%d"), FileLines.Num());
		ExistingContent += ",Draw,Draw,";
		ExistingContent += FString::Printf(TEXT("%s\n"), *FString::FromInt(MoveCounter));
		FFileHelper::SaveStringToFile(ExistingContent, *CSVFilePath);

		return;
	}
	
	// Checkmate
	if (IsCheckMate())
	{
		FFileHelper::LoadFileToString(ExistingContent, *CSVFilePath);

		TArray<FString> FileLines;
		ExistingContent.ParseIntoArrayLines(FileLines);

		FString Player1 = GMode->CurrentPlayer == Player::Player1 ? "Win" : "Lose";
		FString Player2 = GMode->CurrentPlayer == Player::AI ? "Win" : "Lose";

		IsGameOver = true;
		Visible = false;
		IsBotPlayed = true;

		DisableButtonEvent.Broadcast();

		ResetLegalMoveArray();

		// if there are no more legal moves and the king is not in check, it is a draw
		auto Pieces = GMode->CurrentPlayer == Player::AI ? Field->GetAIPieces() : Field->GetPlayer1Pieces();
		for (auto Piece : Pieces)
		{
			if (Piece->LegalMove(Board, GMode->CurrentPlayer, true))
			{
				GMode->Players[GMode->CurrentPlayer]->OnWin();

				// Add victory information to the file
				ExistingContent += FString::Printf(TEXT("%d,%s,%s,%s\n"), FileLines.Num(), *Player1, *Player2, *FString::FromInt(MoveCounter));
				FFileHelper::SaveStringToFile(ExistingContent, *CSVFilePath);

				return;
			}		
		}

		GMode->Players[GMode->CurrentPlayer]->OnDraw();

		// Add draw information to the file
		ExistingContent += FString::Printf(TEXT("%d"), FileLines.Num());
		ExistingContent += ",Draw,Draw,";
		ExistingContent += FString::Printf(TEXT("%s\n"), *FString::FromInt(MoveCounter));

		FFileHelper::SaveStringToFile(ExistingContent, *CSVFilePath);
		
		return;
	}

	GMode->TurnNextPlayer();
}


/*
* @param: move number
* @return: none
* @note: Set all the captured and promoted pieces hidden. 
*        Then retrace all the plays up to the move counter
*        and make the pieces visible/hidden based on promotions and captures
*/
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

		if (ArrayOfPlays[i].Castling)
		{
			if (MoveNumber < ArrayOfPlays.Num())
				MoveNumber++;
		}
		FVector NewLocation = Field->GetActorLocation() + FVector(Position.X, Position.Y, 0.0f);
		NewLocation = Field->GetRelativeLocationByXYPosition(NewLocation.X, NewLocation.Y);
		Piece->SetActorLocation(NewLocation);
	}
}

// Disable input and call RewindManager
void AManagePiece::Replay()
{
	auto PlayerController = Cast<AChess_PlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PlayerController)
		PlayerController->DisableInput(PlayerController);

	RewindManager(ButtonValue + 1);
}

// Call RewindManager to backup the last move and enable input
void AManagePiece::BackToPlay()
{
	RewindManager(ArrayOfPlays.Num());

	auto PlayerController = Cast<AChess_PlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	if (PlayerController)
		PlayerController->EnableInput(PlayerController);
}


/*
* @param: 1.) Pointer to the piece to promote
*         2.) String representing the new class of the piece
* 
* @return: none
* 
* @note: remove the pawn and spawn the new piece represented by the string
*/
void AManagePiece::SpawnNewPiece(AChessPieces* PieceToPromote, FString NewPiece)
{
	AChess_GameMode* GMode = FGameRef::GetGameMode(this);
	AGameField* Field = nullptr;

	if (!GMode || !FGameRef::GetGameField(this, Field, "ManagePiece"))
		return;

	TMap<FVector2D, ATile*> TileMap = Field->GetTileMap();
	TSubclassOf<AChessPieces> PieceClass;

	// Remove the pawn
	auto Position = PieceToPromote->GetGridPosition();
	PieceToPromote->SetActorHiddenInGame(true);
	PieceToPromote->SetActorEnableCollision(false);

	FRewind Obj;
	Obj.PieceToRewind = PieceToPromote;
	Obj.Position = PieceToPromote->GetGridPosition();
	Obj.Capture = true;
	ArrayOfPlays.Add(Obj);

	int32 Player = GMode->CurrentPlayer;
	
	Field->PiecesMapRemove(Position);
	CapturedPieces.Add(PieceToPromote);

	if (PieceToPromote->Color == EPieceColor::BLACK)
		Field->AIPiecesRemove(PieceToPromote);
	else
		Field->Player1PiecesRemove(PieceToPromote);

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

	if (Player == Player::AI)
		Color = EPieceColor::BLACK;
	
	Field->GenerateChessPieceInXYPosition(Position.X, Position.Y, PieceClass, Color);

	auto PiecesMap = Field->GetPiecesMap();
	if (PiecesMap.Contains(Position))
		PromotePieces.Add(PiecesMap[Position]);

	// If it is the human player, the widget representing the piece choice must be removed
	if (Player == Player::Player1)
	{
		auto PlayerController = Cast<AChess_PlayerController>(UGameplayStatics::GetPlayerController(this, 0));

		if (PlayerController)
			PlayerController->RemoveWidgetPawnPromotionToViewport();
		else
			UE_LOG(LogTemp, Error, TEXT("Controller null in ManagePiece"));
	}
	PieceToPromote = nullptr;

	CheckWinAndGoNextPlayer();
}

