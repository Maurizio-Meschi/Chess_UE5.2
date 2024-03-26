// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Chess_Minimax.h"

// Sets default values
AChess_Minimax::AChess_Minimax()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GameInstance = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	PieceColor = EPieceColor::BLACK;
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

	if (IsPieceChoosen)
	{
		IsPieceChoosen = false;
		MovePiece();
	}
}

void AChess_Minimax::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

/*
* @param: none
* @return: none
* @note: It chooses the best move and sets the IsPieceChoosen variable true,
*        so the tick takes care of calling the move piece
*		 The calculation of the best move is carried out on a different thread from the GameThread to avoid UI blocks
*/
void AChess_Minimax::OnTurn()
{
	GameInstance->SetTurnMessage(TEXT("AI (Minimax) Turn"));
		
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [&]()
		{
			// Get reference to game mode, game field and Piece manager
			AChess_GameMode* GMode = nullptr;
			AGameField* GField = nullptr;
			AManagePiece* PieceManager = nullptr;
			if (!FGameRef::GetGameRef(this, GMode, GField, PieceManager, "Minimax"))
				return;

			// Create Board structure
			FBoard Board;
			Board.Field = GField->GetTileMap();
			Board.Pieces = GField->GetPiecesMap();

			// Get the best tile to move the piece
			TileToMove = FindBestMove(Board);

			PieceToMove = Board.PieceToMove;
			IsPieceChoosen = true;
		});
}

/*
* @param: reference to the chessboard
* 
* @return: value of the board
* 
* @note: Evaluation of the chessboard according to four parameters:
*          1.) Value of the pieces present
*          2.) Number of legal moves available
*          3.) Enemy king in check
*          4.) Checkmate
*/
int32 AChess_Minimax::EvaluateGrid(FBoard& Board)
{
	AChess_GameMode* GMode = nullptr;
	AGameField* GField = nullptr;
	AManagePiece* PieceManager = nullptr;
	if (!FGameRef::GetGameRef(this, GMode, GField, PieceManager, "Minimax"))
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
		if (Piece->Color == (Board.IsMax ? EPieceColor::WHITE : EPieceColor::BLACK))
			Score -= Piece->Value;
		else
			Score += Piece->Value;
	}

	// 2) Check if the opponent's king is in check
	
	auto PiecesArray = (Board.IsMax ? GField->GetBotPieces() : GField->GetHumanPlayerPieces());
	for (auto Piece : PiecesArray)
	{
		if (!Board.CapturedPieces.Contains(Piece))
		{
			if (Piece->LegalMove(Board, Board.IsMax ? Player::AI : Player::HUMAN, true))
				Score += (Board.IsMax ? 7 : -7);
		}
	}
	
	/*
	3) Consider the number of legal moves available:
		1    -> very bad
		2-4  -> bad
		4-8  -> good
		8-12 -> very good
		12+  -> excellent
	*/
	auto LegalMoveArray = PieceManager->GetAllLegalMoveByPlayer(Board, Board.IsMax ? Player::AI : Player::HUMAN);;

	int32 MoveCount = 0;
	for (auto TileMarked : LegalMoveArray)
	{
		for (auto Tile : TileMarked)
		{
			MoveCount++;
		}
	}
	
	if (MoveCount == 1)
		Score += (Board.IsMax ? 1 : -1);

	else if (MoveCount >= 2 && MoveCount <= 4)
		Score += (Board.IsMax ? 2 : -2);

	else if (MoveCount > 4 && MoveCount <= 8)
		Score += (Board.IsMax ? 3 : -3);

	else if (MoveCount > 8 && MoveCount <= 12)
		Score += (Board.IsMax ? 6 : -6);

	else if (MoveCount > 12)
		Score += (Board.IsMax ? 10 : -10);
	

	// 4) Check if is Checkmate
	LegalMoveArray = PieceManager->GetAllLegalMoveByPlayer(Board, !Board.IsMax ? Player::AI : Player::HUMAN);

	// se non ci sono più legal move restituisco direttamente il minimo valore (ho perso)
	int32 Cont = 0;

	for (int32 i = 0; i < LegalMoveArray.Num(); i++)
	{
		if (LegalMoveArray[i].Num() > 0)
			break;
		else
			Cont++;
	}

	if (Cont == LegalMoveArray.Num())
		return (Board.IsMax ? 10000 : -10000);
	
	return Score;
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
	int Score = EvaluateGrid(Board);

	if (Depth == 2)
		return Score;

	if (Score == 10000 || Score == -10000)
		return Score;

	AChess_GameMode* GMode = nullptr;
	AGameField* GField = nullptr;
	AManagePiece* PieceManager = nullptr;
	if (!FGameRef::GetGameRef(this, GMode, GField, PieceManager, "Minimax"))
		return 0;

	// Reset legalMove
	PieceManager->ResetLegalMoveArray();

	// If this maximizer's move
	if (IsMax)
	{
		int32 best = -1000;

		Board.IsMax = true;

		auto PlayerArray = GField->GetBotPieces();
		// Iterate for each piece of the AI
		for (auto Piece : PlayerArray)
		{
			TArray<FMarked> TileMarked;

			// Get the array of legal moves for the piece
			if (!Board.CapturedPieces.Contains(Piece))
				TileMarked = PieceManager->GetLegalMoveByPiece(Board, Player::AI, Piece);

			// If the piece has no legal moves or has been captured, go to the next piece
			if (TileMarked.Num() == 0 || Board.CapturedPieces.Contains(Piece))
				continue;

			// Iterate for each legal move
			for (auto Marked : TileMarked)
			{
				ATile* CurrTile = nullptr;
				//FVector2D StartPosition = Piece->GetGridPosition();

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
				Marked.Tile->SetTileStatus(Player::AI, ETileStatus::OCCUPIED);

				// Move the piece
				Board.Pieces.Add(Marked.Tile->GetGridPosition(), Piece);
				Piece->SetGridPosition(Marked.Tile->GetGridPosition().X, Marked.Tile->GetGridPosition().Y);

				// Call the minimax recursively
				best = FMath::Max(best, MiniMax(Board, Depth + 1, alpha, beta, !IsMax));

				/// Restore data structures.
				
				// Set the current tile occupied
				CurrTile->SetTileStatus(Player::AI, ETileStatus::OCCUPIED);

				Board.Pieces.Add(CurrTile->GetGridPosition(), Piece);

				//Set empty the tile where the piece had moved
				Marked.Tile->SetTileStatus(-1, ETileStatus::EMPTY);

				Board.Pieces.Remove(Marked.Tile->GetGridPosition());
				Piece->SetGridPosition(CurrTile->GetGridPosition().X, CurrTile->GetGridPosition().Y);

				// If an enemy piece had been captured, set the tile status occupied by the human
				if (PieceToCapture)
				{
					Board.CapturedPieces.Remove(PieceToCapture);
					Board.Pieces.Add(PieceToCapture->GetGridPosition(), PieceToCapture);
					Marked.Tile->SetTileStatus(Player::HUMAN, ETileStatus::OCCUPIED);
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
		int32 best = 1000;

		Board.IsMax = false;

		auto PlayerArray = GField->GetHumanPlayerPieces();

		// Iterate for each piece of the Human player
		for (auto Piece : PlayerArray)
		{
			TArray<FMarked> TileMarked;

			// Get the array of legal moves for the piece
			if (!Board.CapturedPieces.Contains(Piece))
				TileMarked = PieceManager->GetLegalMoveByPiece(Board, Player::HUMAN, Piece);

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

				// Sets the tile the piece moves to occupied by the Human
				Marked.Tile->SetTileStatus(Player::HUMAN, ETileStatus::OCCUPIED);

				Board.Pieces.Add(Marked.Tile->GetGridPosition(), Piece);
				Piece->SetGridPosition(Marked.Tile->GetGridPosition().X, Marked.Tile->GetGridPosition().Y);
				
				// Call the minimax recursively
				best = FMath::Min(best, MiniMax(Board, Depth + 1, alpha, beta, !IsMax));

				/// Restore data structures.

				// Set the current tile occupied
				CurrTile->SetTileStatus(Player::HUMAN, ETileStatus::OCCUPIED);

				Board.Pieces.Add(CurrTile->GetGridPosition(), Piece);

				//Set empty the tile where the piece had moved
				Marked.Tile->SetTileStatus(-1, ETileStatus::EMPTY);

				Board.Pieces.Remove(Marked.Tile->GetGridPosition());
				Piece->SetGridPosition(CurrTile->GetGridPosition().X, CurrTile->GetGridPosition().Y);

				// If an enemy piece had been captured, set the tile status occupied by the AI
				if (PieceToCapture)
				{
					Board.CapturedPieces.Remove(PieceToCapture);
					Board.Pieces.Add(Marked.Tile->GetGridPosition(), PieceToCapture);
					Marked.Tile->SetTileStatus(Player::AI, ETileStatus::OCCUPIED);
				}
				
				beta = FMath::Min(beta, best);
				if (beta <= alpha)
				{
					//UE_LOG(LogTemp, Error, TEXT("Poto nel min"));
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

	int32 bestVal = -1000;
	FMarked TileToMovePiece;

	Board.IsMax = true;

	// Reset legalMove
	PieceManager->ResetLegalMoveArray();

	auto PlayerArray = GField->GetBotPieces();

	// Iterate for each piece of AI
	for (auto Piece : PlayerArray)
	{
		TArray<FMarked> TileMarked;

		if (!Board.CapturedPieces.Contains(Piece))
			TileMarked = PieceManager->GetLegalMoveByPiece(Board, Player::AI, Piece);

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

			Marked.Tile->SetTileStatus(Player::AI, ETileStatus::OCCUPIED);

			Board.Pieces.Add(Marked.Tile->GetGridPosition(), Piece);
			Piece->SetGridPosition(Marked.Tile->GetGridPosition().X, Marked.Tile->GetGridPosition().Y);

			// Call the minimax algorithm
			int32 moveVal = MiniMax(Board, 0, -1000, 1000, false);

			/// Restore data structures.

			CurrTile->SetTileStatus(Player::AI, ETileStatus::OCCUPIED);

			Board.Pieces.Add(CurrTile->GetGridPosition(), Piece);

			Marked.Tile->SetTileStatus(-1, ETileStatus::EMPTY);
			Board.Pieces.Remove(Marked.Tile->GetGridPosition());
			Piece->SetGridPosition(CurrTile->GetGridPosition().X, CurrTile->GetGridPosition().Y);

			if (PieceToCapture)
			{
				Board.CapturedPieces.Remove(PieceToCapture);
				Board.Pieces.Add(Marked.Tile->GetGridPosition(), PieceToCapture);
				Marked.Tile->SetTileStatus(Player::HUMAN, ETileStatus::OCCUPIED);
			}

			// check if it's the best move
			if (moveVal >= bestVal)
			{
				TileToMovePiece = Marked;
				Board.PieceToMove = Piece;
				bestVal = moveVal;
			}
			
		}
	}

	return TileToMovePiece;
}

void AChess_Minimax::MovePiece()
{
	AChess_GameMode* GMode = nullptr;
	AGameField* GField = nullptr;
	AManagePiece* PieceManager = nullptr;

	if (!FGameRef::GetGameRef(this, GMode, GField, PieceManager, "Minimax"))
		return;

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

	// Before moving the piece, set the current tile empty
	if (GField->GetTileMap().Contains(PieceToMove->GetGridPosition()))
		GField->GetTileMap()[PieceToMove->GetGridPosition()]->SetTileStatus(PlayerNumber, ETileStatus::EMPTY);

	PieceManager->MovePiece(Player::AI, PieceToMove, Position, PieceToMove->GetGridPosition());
}

void AChess_Minimax::OnWin()
{
	GameInstance->SetTurnMessage(TEXT("AI Wins!"));
}

void AChess_Minimax::OnLose()
{
	GameInstance->SetTurnMessage(TEXT("AI Loses!"));
}
