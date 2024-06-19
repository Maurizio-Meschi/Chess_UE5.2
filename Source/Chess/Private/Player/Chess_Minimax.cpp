// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Chess_Minimax.h"
#include "Player/Minimax_Element/PieceSquareTables.h"

// Sets default values
AChess_Minimax::AChess_Minimax()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GameInstance = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
}

// Called when the game starts or when spawned
void AChess_Minimax::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChess_Minimax::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AChess_Minimax::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

/*
* @param: none
* @return: none
* @note: Sets a timer and when the time runs out 
*        it chooses the best move
*/
void AChess_Minimax::OnTurn()
{
	GameInstance->SetTurnMessage(TEXT("AI (Minimax) Turn"));
		
	// Get reference to Piece manager
	AManagePiece* ManagerPiece = nullptr;

	if (!FGameRef::GetManagePiece(this,ManagerPiece, "Minimax"))
		return;

	// Set timer
	GetWorld()->GetTimerManager().SetTimer(ManagerPiece->TimerHandle, [&]()
		{
			// Get reference to game mode, game field and Piece manager
			AChess_GameMode* GMode = nullptr;
			AGameField* GField = nullptr;
			AManagePiece* PieceManager = nullptr;
			if (!FGameRef::GetGameRef(this, GMode, GField, PieceManager, "Minimax"))
				return;

			FBoard Board;
			Board.Field = GField->GetTileMap();
			Board.Pieces = GField->GetPiecesMap();

			FMarked TileToMove = FindBestMove(Board);
			
			AChessPieces* PieceToMove = Board.PieceToMove;

			if (!PieceToMove || !TileToMove.Tile)
			{
				UE_LOG(LogTemp, Error, TEXT("No Piece or tile"));
				return;
			}
			FVector2D Position = TileToMove.Tile->GetGridPosition();
			// Check if possible to capture an enemy piece
			if (TileToMove.Capture)
			{
				TileToMove.Tile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);

				AChessPieces* PieceToCapture = nullptr;
				if (GField->GetPiecesMap().Contains(Position))
					PieceToCapture = GField->GetPiecesMap()[(Position)];

				PieceManager->CapturePiece(PieceToCapture, Position);
			}

			TileToMove.Tile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);

			// Before moving the piece, set the current tile to be empty
			if (GField->GetTileMap().Contains(PieceToMove->GetGridPosition()))
				GField->GetTileMap()[PieceToMove->GetGridPosition()]->SetTileStatus(-1, ETileStatus::EMPTY);
			else
				UE_LOG(LogTemp, Error, TEXT("No tile"));

			PieceManager->Move++;
			PieceManager->MovePiece(PlayerNumber, PieceToMove, Position, PieceToMove->GetGridPosition());
		}, 0.5, false);
}

/*
* @param: reference to the chessboard
* 
* @return: value of the board
* 
* @note: Evaluation of the chessboard according to two parameters:
*          1.) Value of the pieces present
*          2.) Enemy king in check
*/
int32 AChess_Minimax::EvaluateGrid(FBoard& Board)
{
	AGameField* GField = nullptr;
	
	if (!FGameRef::GetGameField(this, GField, "Minimax"))
		return 0;

	int32 Score = 0;

	/*
	1) Evaluation of all pieces. 
	   If the piece belongs to the opposing player, 
	   the value of the piece is subtracted from the score, otherwise it is added
	*/
	for (auto Element : Board.Pieces)
	{
		auto Piece = Element.Value;
		if (PlayerNumber == Player::AI)
		{
			if (Piece->Color == EPieceColor::WHITE)
				Score -= Piece->Value;
			else
				Score += Piece->Value;
		}
		else
		{ 
			if (Piece->Color == EPieceColor::BLACK)
				Score -= Piece->Value;
			else
				Score += Piece->Value;
		}
	}

	// 2) Check if the opponent's king is in check

	TArray<AChessPieces*> PiecesArray; 

	if (PlayerNumber == Player::AI)
		PiecesArray = Board.IsMax ? GField->GetAIPieces() : GField->GetPlayer1Pieces();

	else
		PiecesArray = Board.IsMax ? GField->GetPlayer1Pieces() : GField->GetAIPieces();

	for (auto Piece : PiecesArray)
	{
		if (!Board.CapturedPieces.Contains(Piece))
		{
			int32 EnemyPlayer = PlayerNumber == Player::Player1 ? Player::AI : Player::Player1;
			if (Piece->LegalMove(Board, Board.IsMax ? PlayerNumber : EnemyPlayer, true))
				Score += (Board.IsMax ? DRAW_VALUE : -DRAW_VALUE);
		}
	}

	return Score;
}

int32 AChess_Minimax::EvaluateGridAdvanced(FBoard& Board)
{
	AGameField* GField = nullptr;
	AManagePiece* PieceManager = nullptr;

	if (!FGameRef::GetGameField(this, GField, "Minimax") || !FGameRef::GetManagePiece(this, PieceManager, "Minimax"))
		return 0;

	int32 Score = 0;

	bool IsMiddleGame = true;

	if (Board.Pieces.Num() < 12)
		IsMiddleGame = false;

	for (auto Element : Board.Pieces)
	{
		auto Piece = Element.Value;
		int32 x = Piece->GetGridPosition().X;
		int32 y = Piece->GetGridPosition().Y;


		if (PlayerNumber == Player::AI)
		{
			if (Piece->Color == EPieceColor::WHITE)
			{
				Score -= IsMiddleGame ? Piece->Value : Piece->EGValue;
				Score -= PieceSquareTables::PieceValue(x, y, Piece->Name, IsMiddleGame, EPieceColor::WHITE);
				if (Piece->IsA<AChessPawn>() && x == 7)
					Score -= PAWN_PROMOTION_VALUE;
			}
			else
			{
				Score += IsMiddleGame ? Piece->Value : Piece->EGValue;
				Score += PieceSquareTables::PieceValue(x, y, Piece->Name, IsMiddleGame, EPieceColor::BLACK);
				if (Piece->IsA<AChessPawn>() && x == 0)
					Score += PAWN_PROMOTION_VALUE;
			}
		}
		else
		{
			if (Piece->Color == EPieceColor::BLACK)
			{
				Score -= IsMiddleGame ? Piece->Value : Piece->EGValue;
				Score -= PieceSquareTables::PieceValue(x, y, Piece->Name, IsMiddleGame, EPieceColor::BLACK);
				if (Piece->IsA<AChessPawn>() && x == 0)
					Score -= PAWN_PROMOTION_VALUE;
			}
			else
			{
				Score += IsMiddleGame ? Piece->Value : Piece->EGValue;
				Score += PieceSquareTables::PieceValue(x, y, Piece->Name, IsMiddleGame, EPieceColor::WHITE);
				if (Piece->IsA<AChessPawn>() && x == 7)
					Score += PAWN_PROMOTION_VALUE;
			}
		}
	}

	// 2) Check if the opponent's king is in check to improve start game
	
	if (PieceManager->Move < 2)
	{
		TArray<AChessPieces*> PiecesArray;

		if (PlayerNumber == Player::AI)
			PiecesArray = Board.IsMax ? GField->GetAIPieces() : GField->GetPlayer1Pieces();

		else
			PiecesArray = Board.IsMax ? GField->GetPlayer1Pieces() : GField->GetAIPieces();

		for (auto Piece : PiecesArray)
		{
			if (!Board.CapturedPieces.Contains(Piece))
			{
				int32 EnemyPlayer = PlayerNumber == Player::Player1 ? Player::AI : Player::Player1;
				if (Piece->LegalMove(Board, Board.IsMax ? PlayerNumber : EnemyPlayer, true))
					Score += (Board.IsMax ? DRAW_VALUE : -DRAW_VALUE);
			}
		}
	}

	return Score;
}


/*
* @param: reference to the chessboard
*
* @return: true if there are no more legal moves available, false otherwise
*
* @note: none
*/
bool AChess_Minimax::Checkmate(FBoard& Board)
{
	AManagePiece* PieceManager = nullptr;
	if (!FGameRef::GetManagePiece(this, PieceManager, "minimax"))
		return false;
	int32 EnemyPlayer = PlayerNumber == Player::Player1 ? Player::AI : Player::Player1;
	auto LegalMoveArray = PieceManager->GetAllLegalMoveByPlayer(Board, !Board.IsMax ? PlayerNumber : EnemyPlayer);

	int32 Cont = 0;

	for (int32 i = 0; i < LegalMoveArray.Num(); i++)
	{
		if (LegalMoveArray[i].Num() > 0)
			break;
		else
			Cont++;
	}

	if (Cont == LegalMoveArray.Num())
		return true;

	return false;
}

/*
* @param: 1.) Reference to the chessboard
*         2.) Depth
*         3.) Alpha (best Max value found)
*         4.) Beta  (best Min value found)
*         5.) IsMax (maximizer or the minimizer turn)
* 
* @return: best value
* 
* @note: classic minimax algorithm
*/
int32 AChess_Minimax::MiniMax(FBoard& Board, int32 Depth, int32 alpha, int32 beta, bool IsMax)
{
	AChess_GameMode* GMode = nullptr;
	AGameField* GField = nullptr;
	AManagePiece* PieceManager = nullptr;
	if (!FGameRef::GetGameRef(this, GMode, GField, PieceManager, "Minimax"))
		return 0;

	if (Depth == 2)
	{
		if (GameInstance && GameInstance->ChooseAiPlayer == "BaseAI vs AdvancedAI") {
			if (PlayerNumber == Player::AI)
				return EvaluateGridAdvanced(Board);
			else
				return EvaluateGrid(Board);
		}
		else
			return EvaluateGridAdvanced(Board);
	}
	

	if (Checkmate(Board))
		return (Board.IsMax ? 10000 : -10000);

	if (GMode->IsDraw(Board))
		return (Board.IsMax ? -1000 : 1000);

	GMode->FEN_Array.RemoveAt(GMode->FEN_Array.Num() - 1);

	// Reset legalMove
	PieceManager->ResetLegalMoveArray();

	// If this maximizer's move
	if (IsMax)
	{
		int32 best = -10000;

		Board.IsMax = true;

		auto PlayerArray = PlayerNumber == Player::Player1 ? GField->GetPlayer1Pieces() : GField->GetAIPieces();
		// Iterate for each piece of the AI
		for (auto Piece : PlayerArray)
		{
			TArray<FMarked> TileMarked;

			// Get the array of legal moves for the piece
			if (!Board.CapturedPieces.Contains(Piece))
				TileMarked = PieceManager->GetLegalMoveByPiece(Board, PlayerNumber, Piece);

			// If the piece has no legal moves or has been captured, go to the next piece
			if (TileMarked.Num() == 0 || Board.CapturedPieces.Contains(Piece))
				continue;

			// Iterate for each legal move
			for (auto Marked : TileMarked)
			{
				ATile* CurrTile = nullptr;

				if (Board.Field.Contains(Piece->GetGridPosition()))
					CurrTile = Board.Field[Piece->GetGridPosition()];

				// Set the tile where the piece is located empty
				CurrTile->SetTileStatus(-1, ETileStatus::EMPTY);
				Board.Pieces.Remove(CurrTile->GetGridPosition());

				// If the tile is occupied allocate the piece in the array of captured pieces
				AChessPieces* PieceToCapture = nullptr;
				if (Marked.Capture)
				{
					// Prendo il pezzo da catturare
					if (Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
					{
						PieceToCapture = Board.Pieces[Marked.Tile->GetGridPosition()];
						Board.CapturedPieces.Add(PieceToCapture);
						Board.Pieces.Remove(PieceToCapture->GetGridPosition());
					}
					else
						UE_LOG(LogTemp, Error, TEXT("PieceToCapture null in max"));
				}

				// Sets the tile the piece moves to occupied by the AI
				Marked.Tile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);

				// Move the piece
				Board.Pieces.Add(Marked.Tile->GetGridPosition(), Piece);
				Piece->SetGridPosition(Marked.Tile->GetGridPosition().X, Marked.Tile->GetGridPosition().Y);

				// Call the minimax recursively
				best = FMath::Max(best, MiniMax(Board, Depth + 1, alpha, beta, !IsMax));

				/// Restore data structures.

				// Set the current tile occupied
				CurrTile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);

				Board.Pieces.Add(CurrTile->GetGridPosition(), Piece);

				// Set empty the tile where the piece had moved
				Marked.Tile->SetTileStatus(-1, ETileStatus::EMPTY);

				Board.Pieces.Remove(Marked.Tile->GetGridPosition());
				Piece->SetGridPosition(CurrTile->GetGridPosition().X, CurrTile->GetGridPosition().Y);

				// If an enemy piece had been captured, set the tile status occupied by the enemy
				if (PieceToCapture)
				{
					Board.CapturedPieces.Remove(PieceToCapture);
					Board.Pieces.Add(PieceToCapture->GetGridPosition(), PieceToCapture);
					Marked.Tile->SetTileStatus(PlayerNumber == Player::AI ? Player::Player1 : Player::AI, ETileStatus::OCCUPIED);
				}
				
				alpha = FMath::Max(alpha, best);
				if (beta <= alpha)
				{
					break;
				}
			}
		}
		return best;
	}

	// If this minimizer's move
	else
	{
		int32 best = 10000;

		Board.IsMax = false;

		auto PlayerArray = PlayerNumber == Player::Player1 ? GField->GetAIPieces() : GField->GetPlayer1Pieces();

		// Iterate for each piece of the Human player
		for (auto Piece : PlayerArray)
		{
			TArray<FMarked> TileMarked;
			int32 EnemyPlayer = PlayerNumber == Player::AI ? Player::Player1 : Player::AI;

			// Get the array of legal moves for the piece
			if (!Board.CapturedPieces.Contains(Piece))
				TileMarked = PieceManager->GetLegalMoveByPiece(Board, EnemyPlayer, Piece);

			// If the piece has no legal moves or has been captured, go to the next piece
			if (TileMarked.Num() == 0 || Board.CapturedPieces.Contains(Piece))
				continue;

			// Iterate for each legal move
			for (auto Marked : TileMarked)
			{
				ATile* CurrTile = nullptr;

				if (Board.Field.Contains(Piece->GetGridPosition()))
					CurrTile = Board.Field[Piece->GetGridPosition()];

				// Set the tile where the piece is located empty
				CurrTile->SetTileStatus(-1, ETileStatus::EMPTY);
				Board.Pieces.Remove(CurrTile->GetGridPosition());

				// If the tile is occupied allocate the piece in the array of captured pieces
				AChessPieces* PieceToCapture = nullptr;
				if (Marked.Capture)
				{
					if (Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
					{
						PieceToCapture = Board.Pieces[Marked.Tile->GetGridPosition()];
						Board.CapturedPieces.Add(PieceToCapture);
					}
					else
						UE_LOG(LogTemp, Error, TEXT("PieceToCapture null minmax"));
				}

				// Sets the tile the piece moves to occupied by the enemy
				Marked.Tile->SetTileStatus(EnemyPlayer, ETileStatus::OCCUPIED);

				Board.Pieces.Add(Marked.Tile->GetGridPosition(), Piece);
				Piece->SetGridPosition(Marked.Tile->GetGridPosition().X, Marked.Tile->GetGridPosition().Y);

				// Call the minimax recursively
				best = FMath::Min(best, MiniMax(Board, Depth + 1, alpha, beta, !IsMax));

				/// Restore data structures.

				// Set the current tile occupied
				CurrTile->SetTileStatus(EnemyPlayer, ETileStatus::OCCUPIED);

				Board.Pieces.Add(CurrTile->GetGridPosition(), Piece);

				// Set empty the tile where the piece had moved
				Marked.Tile->SetTileStatus(-1, ETileStatus::EMPTY);

				Board.Pieces.Remove(Marked.Tile->GetGridPosition());
				Piece->SetGridPosition(CurrTile->GetGridPosition().X, CurrTile->GetGridPosition().Y);

				// If an enemy piece had been captured, set the tile status occupied by the player
				if (PieceToCapture)
				{
					Board.CapturedPieces.Remove(PieceToCapture);
					Board.Pieces.Add(Marked.Tile->GetGridPosition(), PieceToCapture);
					Marked.Tile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);
				}
				
				beta = FMath::Min(beta, best);
				if (beta <= alpha)
				{
					break;
				}
			}
		}
		return best;
	}
}


/*
* @param: reference to the chessboard
*
* @return: FMarked obj contains the best tile to move the piece to 
*		   and whether the tile contains an enemy piece to capture
*
* @note: start the minimax algorithm
*/
FMarked AChess_Minimax::FindBestMove(FBoard& Board)
{
	AChess_GameMode* GMode = nullptr;
	AGameField* GField = nullptr;
	AManagePiece* PieceManager = nullptr;
	FGameRef::GetGameRef(this, GMode, GField, PieceManager, "Minimax");

	int32 bestVal = -100000;
	FMarked TileToMovePiece{};

	Board.IsMax = true;

	// Reset legalMove
	PieceManager->ResetLegalMoveArray();

	auto PlayerArray = PlayerNumber == Player::AI ? GField->GetAIPieces() : GField->GetPlayer1Pieces();

	// Iterate for each piece of AI
	for (auto Piece : PlayerArray)
	{
		TArray<FMarked> TileMarked;

		if (!Board.CapturedPieces.Contains(Piece))
			TileMarked = PieceManager->GetLegalMoveByPiece(Board, PlayerNumber, Piece);

		if (TileMarked.Num() == 0)
			continue;

		// Iterate for each legal move
		for (auto Marked : TileMarked)
		{
			// Simulate movement in the marked tile
			ATile* CurrTile = nullptr;

			if (Board.Field.Contains(Piece->GetGridPosition()))
				CurrTile = Board.Field[Piece->GetGridPosition()];

			CurrTile->SetTileStatus(-1, ETileStatus::EMPTY);
			Board.Pieces.Remove(CurrTile->GetGridPosition());

			AChessPieces* PieceToCapture = nullptr;
			if (Marked.Capture)
			{
				if (Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
				{
					PieceToCapture = Board.Pieces[Marked.Tile->GetGridPosition()];
					Board.CapturedPieces.Add(PieceToCapture);
				}
				else
					UE_LOG(LogTemp, Error, TEXT("PieceToCapture null bestmove"));
			}

			Marked.Tile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);

			Board.Pieces.Add(Marked.Tile->GetGridPosition(), Piece);
			Piece->SetGridPosition(Marked.Tile->GetGridPosition().X, Marked.Tile->GetGridPosition().Y);

			
			int32 moveVal = 0;

			moveVal = MiniMax(Board, 0, -100000, 100000, false);
			
			/// Restore data structures.

			CurrTile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);

			Board.Pieces.Add(CurrTile->GetGridPosition(), Piece);

			Marked.Tile->SetTileStatus(-1, ETileStatus::EMPTY);
			Board.Pieces.Remove(Marked.Tile->GetGridPosition());
			Piece->SetGridPosition(CurrTile->GetGridPosition().X, CurrTile->GetGridPosition().Y);

			if (PieceToCapture)
			{
				Board.CapturedPieces.Remove(PieceToCapture);
				Board.Pieces.Add(Marked.Tile->GetGridPosition(), PieceToCapture);
				Marked.Tile->SetTileStatus(PlayerNumber == Player::AI ? Player::Player1 : Player::AI, ETileStatus::OCCUPIED);
			}

			// Check if it's the best move
			// If several moves have the same value, it randomly chooses among them
			if (moveVal > bestVal || (moveVal == bestVal && FMath::Rand() % 2 != 0))
			{
				TileToMovePiece = Marked;
				Board.PieceToMove = Piece;
				bestVal = moveVal;
			}	
		}
	}
	
	return TileToMovePiece;
}

void AChess_Minimax::OnWin()
{
	FString tmp = PlayerNumber == Player::Player1 ? "(White)" : "(Balck)";
	GameInstance->SetTurnMessage(*("AI " + tmp + " Wins!"));
}

void AChess_Minimax::OnDraw()
{
	GameInstance->SetTurnMessage(TEXT("Draw!"));
}
