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

}

void AChess_Minimax::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AChess_Minimax::OnTurn()
{
	GameInstance->SetTurnMessage(TEXT("AI (Minimax) Turn"));

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			AChess_GameMode* GMode = nullptr;
			AGameField* GField = nullptr;
			AManagePiece* PieceManager = nullptr;
			if (!FGameRef::GetGameRef(this, GMode, GField, PieceManager, "Minimax"))
				return;

			FBoard Board;
			Board.Field = GField->GetTileMap();
			Board.Pieces = GField->GetPiecesMap();
			UE_LOG(LogTemp, Error, TEXT("Vado a cercare la best move"));

			FMarked TileToMove = FindBestMove(Board);

			FVector2D Position = TileToMove.Tile->GetGridPosition();
			AChessPieces* PieceToMove = Board.PieceToMove;
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
				GField->GetTileMap()[PieceToMove->GetGridPosition()]->SetTileStatus(PlayerNumber, ETileStatus::EMPTY);

			PieceManager->MovePiece(GMode->CurrentPlayer, PieceToMove, Position, PieceToMove->GetGridPosition());

		}, 1, false);
}

int32 AChess_Minimax::EvaluateGrid(FBoard& Board)
{
	AChess_GameMode* GMode = nullptr;
	AGameField* GField = nullptr;
	AManagePiece* PieceManager = nullptr;
	if (!FGameRef::GetGameRef(this, GMode, GField, PieceManager, "Minimax"))
		return 0;

	int32 Score = 0;

	// 1) Devo scorrere tutta la scacchiera. Per ogni tile che ha un pezzo, se esso è nero sommo il suo punteggio, altrimento lo sottraggo
	for (auto Element : Board.Pieces)
	{
		auto Piece = Element.Value;
		if (Piece->Color == EPieceColor::WHITE)
			Score -= Piece->Value;
		else
			Score += Piece->Value;
		//UE_LOG(LogTemp, Error, TEXT("Score = %d"), Score);
	}
	// 2) Devo considerare il numero di legal move disponibili per aumentare il valore 
	//    (Se il nemico ha poche legal move probabilmente è sotto scacco -> valore alto). Se io ho tante legalmove anche valore alto 
	//     Posso inglobarlo nel controllo se è checkmate (zero legal move nemico -> valore 10000, poche legal move nemico -> x punti etc) 
	/*
	auto TileMarkedForPiece = PieceManager->TileMarkedForPiece;
	int32 MoveCount = 0;
	for (auto TileMarked : TileMarkedForPiece)
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
		Score += (Board.IsMax ? 4 : -4);

	else if (MoveCount > 12)
		Score += (Board.IsMax ? 5 : -5);

	// 3) Devo capire quante pedine sono adiacenti al re per valutare la sua protezione
	//auto KingPosition = (Board.IsMax ? GField->GetKingArray()[0]->GetGridPosition() : GField->GetKingArray()[1]->GetGridPosition());
	//auto PiecePosition = Board.NewPieceTile->GetGridPosition();

	// Se il pezzo che ho spostato mette sotto scacco il re è una buona giocata
	//if (FindTileBetweenP1P2(PiecePosition, KingPosition, Board))
		//Score += 10;
		*/

	return Score;
}

bool AChess_Minimax::FindTileBetweenP1P2(const FVector2D& P1, const FVector2D& P2, FBoard& Board)
{
	int32 deltaX = P2.X - P1.X;
	int32 deltaY = P2.Y - P1.Y;
	int32 stepX = (deltaX > 0) ? 1 : -1;
	int32 stepY = (deltaY > 0) ? 1 : -1;

	int32 x = P1.X;
	int32 y = P1.Y;


	// Bresenham algorithm.
	while (x != P2.X || y != P2.Y)
	{
		int32 offsetX = FMath::Abs(x - P1.X);
		int32 offsetY = FMath::Abs(y - P1.Y);

		if (FVector2D(x, y) != P1) {

			ATile* SelectedTile = nullptr;
			if (Board.Field.Contains(FVector2D(x, y)))
			{
				SelectedTile = Board.Field[FVector2D(x, y)];
				if (SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
				{
					return false;
				}
			}
		}

		if (offsetX < FMath::Abs(deltaX))
		{
			x += stepX;
		}
		if (offsetY < FMath::Abs(deltaY))
		{
			y += stepY;
		}
	}
	return true;
}

int32 AChess_Minimax::MiniMax(FBoard& Board, int32 Depth, bool IsMax)
{
	int score = EvaluateGrid(Board);

	//UE_LOG(LogTemp, Error, TEXT("Inizio il min max"));
	if (Depth > 0)
		return score;

	AChess_GameMode* GMode = nullptr;
	AGameField* GField = nullptr;
	AManagePiece* PieceManager = nullptr;
	if (!FGameRef::GetGameRef(this, GMode, GField, PieceManager, "Minimax"))
		return 0;

	auto TileMarkedForPiece = PieceManager->TileMarkedForPiece;

	// Pulisco tutte le legalMove
	for (int32 i = 0; i < TileMarkedForPiece.Num(); i++)
		TileMarkedForPiece[i].Empty();

	// If this maximizer's move
	if (IsMax)
	{
		int32 best = -1000;

		Board.IsMax = true;

		//Board.PlayerArray = GField->GetBotPieces();
		auto PlayerArray = GField->GetBotPieces();
		for (auto Piece : PlayerArray)
		{
			if (Board.CapturePieces.Contains(Piece))
				continue;

			Piece->LegalMove(Board, 1, false);
		}

		TileMarkedForPiece = PieceManager->TileMarkedForPiece;
		
		// se non ci sono più legal move restituisco direttamente il minimo valore (ho perso)
		int32 Cont = 0;

		for (int32 i = 0; i < TileMarkedForPiece.Num(); i++)
		{
			if (TileMarkedForPiece[i].Num() > 0)
				break;
			else
				Cont++;
		}

		if (Cont == TileMarkedForPiece.Num())
			return -10000;

		for (auto Piece : PlayerArray)
		{
			auto TileMarked = PieceManager->TileMarkedForPiece[Piece->IndexArray];

			// Se il pezzo non ha legal move oppure è stato catturato vado al prossimo pezzo
			if (TileMarked.Num() == 0 || Board.CapturePieces.Contains(Piece))
				continue;

			for (auto Marked : TileMarked)
			{
				// Gestisco la tile dove era il pezzo prima di muoverlo
				ATile* CurrTile = nullptr;
				FVector2D StartPosition = Piece->GetGridPosition();

				if (Board.Field.Contains(Piece->GetGridPosition()))
					CurrTile = Board.Field[Piece->GetGridPosition()];

				// Metto empty la tile in cui si sposta il pezzo
				CurrTile->SetTileStatus(-1, ETileStatus::EMPTY);
				Board.Pieces.Remove(CurrTile->GetGridPosition());

				// Se la tile è occupata alloco il pezzo nell'array dei pezzi catturati
				AChessPieces* PieceToCapture = nullptr;
				if (Marked.Capture)
				{
					// Prendo il pezzo da catturare
					if (Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
					{
						PieceToCapture = Board.Pieces[Marked.Tile->GetGridPosition()];
						Board.CapturePieces.Add(PieceToCapture);
						Board.Pieces.Remove(PieceToCapture->GetGridPosition());
					}
					else
						UE_LOG(LogTemp, Error, TEXT("PieceToCapture null"));
				}

				// Metto occupata dal bot la tile in cui mi sposto e sposto il pezzo
				Marked.Tile->SetTileStatus(1, ETileStatus::OCCUPIED);

				if (!Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
				{
					Board.Pieces.Add(Marked.Tile->GetGridPosition(), Piece);
					Piece->SetGridPosition(Marked.Tile->GetGridPosition().X, Marked.Tile->GetGridPosition().Y);
				}

				//UE_LOG(LogTemp, Error, TEXT("Chiamo il min max"));
				best = FMath::Max(best, MiniMax(Board, Depth + 1, !IsMax));
				//UE_LOG(LogTemp, Error, TEXT("Fine chiamata"));

				// Metto la current tile occupata
				CurrTile->SetTileStatus(1, ETileStatus::OCCUPIED);

				if (!Board.Pieces.Contains(CurrTile->GetGridPosition()))
					Board.Pieces.Add(CurrTile->GetGridPosition(), Piece);

				Marked.Tile->SetTileStatus(-1, ETileStatus::EMPTY);
				Board.Pieces.Remove(Piece->GetGridPosition());
				Piece->SetGridPosition(StartPosition.X, StartPosition.Y);

				// Se ho catturato metto la tile occupata dallo human, altrimenti la mettoì empty
				if (PieceToCapture)
				{
					Board.CapturePieces.Remove(PieceToCapture);
					if (!Board.Pieces.Contains(PieceToCapture->GetGridPosition()))
						Board.Pieces.Add(PieceToCapture->GetGridPosition(), PieceToCapture);
					Marked.Tile->SetTileStatus(0, ETileStatus::OCCUPIED);
				}
			}
		}
		//UE_LOG(LogTemp, Error, TEXT("AI (Minimax) bestVal = %d "), best);
		return best;
	}

	// If this minimizer's move
	else
	{
		int32 best = -1000;

		Board.IsMax = false;

		//Board.PlayerArray = GField->GetHumanPlayerPieces();
		auto PlayerArray = GField->GetHumanPlayerPieces();
		for (auto Piece : PlayerArray)
		{
			if (Board.CapturePieces.Contains(Piece))
				continue;

			Piece->LegalMove(Board, 0, false);
		}

		TileMarkedForPiece = PieceManager->TileMarkedForPiece; 

		// se non ci sono più legal move restituisco direttamente il minimo valore (ho vinto)
		int32 Cont = 0;

		for (int32 i = 0; i < TileMarkedForPiece.Num(); i++)
		{
			if (TileMarkedForPiece[i].Num() > 0)
				break;
			else
				Cont++;
		}

		if (Cont == TileMarkedForPiece.Num())
			return 10000;

		for (auto Piece : PlayerArray)
		{
			auto TileMarked = PieceManager->TileMarkedForPiece[Piece->IndexArray];
			// Se il pezzo non ha legal move oppure è stato catturato vado al prossimo pezzo
			if (TileMarked.Num() == 0 || Board.CapturePieces.Contains(Piece))
				continue;

			for (auto Marked : TileMarked)
			{
				// Gestisco la tile dove era il pezzo prima di muoverlo
				ATile* CurrTile = nullptr;
				FVector2D StartPosition = Piece->GetGridPosition();

				if (Board.Field.Contains(Piece->GetGridPosition()))
					CurrTile = Board.Field[Piece->GetGridPosition()];

				// Metto empty la tile in cui si sposta il pezzo
				CurrTile->SetTileStatus(-1, ETileStatus::EMPTY);
				Board.Pieces.Remove(CurrTile->GetGridPosition());

				// Se la tile è occupata alloco il pezzo nell'array dei pezzi catturati
				AChessPieces* PieceToCapture = nullptr;
				if (Marked.Capture)
				{
					//UE_LOG(LogTemp, Error, TEXT("Piece to capture x = %f y = %f"), Marked.Tile->GetGridPosition().X, Marked.Tile->GetGridPosition().Y);
					//UE_LOG(LogTemp, Error, TEXT("Piece name %s x = %f y = %f"),*Piece->GetName(), Piece->GetGridPosition().X, Piece->GetGridPosition().Y);
					// Prendo il pezzo da catturare
					if (Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
					{
						PieceToCapture = Board.Pieces[Marked.Tile->GetGridPosition()];
						Board.CapturePieces.Add(PieceToCapture);
						Board.Pieces.Remove(PieceToCapture->GetGridPosition());
					}
					//else
						//UE_LOG(LogTemp, Error, TEXT("PieceToCapture null minmax"));
				}

				// Metto occupata dallo human la tile in cui mi sposto e sposto il pezzo
				Marked.Tile->SetTileStatus(0, ETileStatus::OCCUPIED);

				if (!Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
				{
					Board.Pieces.Add(Marked.Tile->GetGridPosition(), Piece);
					Piece->SetGridPosition(Marked.Tile->GetGridPosition().X, Marked.Tile->GetGridPosition().Y);
				}

				best = FMath::Max(best, MiniMax(Board, Depth + 1, !IsMax));

				// Metto la current tile occupata
				CurrTile->SetTileStatus(0, ETileStatus::OCCUPIED);

				if (!Board.Pieces.Contains(CurrTile->GetGridPosition()))
					Board.Pieces.Add(CurrTile->GetGridPosition(), Piece);

				Marked.Tile->SetTileStatus(-1, ETileStatus::EMPTY);
				Board.Pieces.Remove(Piece->GetGridPosition());
				Piece->SetGridPosition(StartPosition.X, StartPosition.Y);

				// Se ho catturato metto la tile occupata dallo human, altrimenti la mettoì empty
				if (PieceToCapture)
				{
					Board.CapturePieces.Remove(PieceToCapture);
					if (!Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
						Board.Pieces.Add(Marked.Tile->GetGridPosition(), PieceToCapture);
					Marked.Tile->SetTileStatus(1, ETileStatus::OCCUPIED);
				}
			}
		}
		//UE_LOG(LogTemp, Error, TEXT("AI (Minimax) bestVal = %d "), best);
		return best;
	}
}

FMarked AChess_Minimax::FindBestMove(FBoard& Board)
{
	// faccio la stessa cosa del IsMax -> Chiamo per la prima volta tutte le mosse di tutti i pezzi del bot
	// e mi pesco la giocata con maggior valore
	AChess_GameMode* GMode = nullptr;
	AGameField* GField = nullptr;
	AManagePiece* PieceManager = nullptr;
	FGameRef::GetGameRef(this, GMode, GField, PieceManager, "Minimax");

	int32 bestVal = -1000;
	FMarked TileToMovePiece;

	Board.IsMax = true;

	// Pulisco tutte le legalMove
	auto TileMarkedForPiece = PieceManager->TileMarkedForPiece;

	for (int32 i = 0; i < TileMarkedForPiece.Num(); i++)
		TileMarkedForPiece[i].Empty();

	//Board.PlayerArray = GField->GetBotPieces();
	auto PlayerArray = GField->GetBotPieces();

	for (auto Piece : PlayerArray)
	{
		if (Board.CapturePieces.Contains(Piece))
			continue;

		Piece->LegalMove(Board, 1, false);
	}

	for (auto Piece : PlayerArray)
	{
		//UE_LOG(LogTemp, Error, TEXT("Piece index: %d TileMarkedForPiece.Num = %d"), Piece->IndexArray,TileMarkedForPiece.Num());

		auto TileMarked = PieceManager->TileMarkedForPiece[Piece->IndexArray];

		// Se il pezzo non ha legal move oppure è stato catturato vado al prossimo pezzo
		if (TileMarked.Num() == 0 || Board.CapturePieces.Contains(Piece))
			continue;

		for (auto Marked : TileMarked)
		{
			// Gestisco la tile dove era il pezzo prima di muoverlo
			ATile* CurrTile = nullptr;
			FVector2D StartPosition = Piece->GetGridPosition();

			if (Board.Field.Contains(Piece->GetGridPosition()))
				CurrTile = Board.Field[Piece->GetGridPosition()];

			// Metto empty la tile in cui si sposta il pezzo
			CurrTile->SetTileStatus(-1, ETileStatus::EMPTY);
			Board.Pieces.Remove(CurrTile->GetGridPosition());

			// Se la tile è occupata alloco il pezzo nell'array dei pezzi catturati
			AChessPieces* PieceToCapture = nullptr;
			if (Marked.Capture)
			{
				// Prendo il pezzo da catturare
				if (Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
				{
					PieceToCapture = Board.Pieces[Marked.Tile->GetGridPosition()];
					Board.CapturePieces.Add(PieceToCapture);
					Board.Pieces.Remove(PieceToCapture->GetGridPosition());
				}
				else
					UE_LOG(LogTemp, Error, TEXT("PieceToCapture null bestmove"));
			}

			// Metto occupata dal bot la tile in cui mi sposto e sposto il pezzo
			Marked.Tile->SetTileStatus(1, ETileStatus::OCCUPIED);

			if (!Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
			{
				Board.Pieces.Add(Marked.Tile->GetGridPosition(), Piece);
				Piece->SetGridPosition(Marked.Tile->GetGridPosition().X, Marked.Tile->GetGridPosition().Y);
			}
			
			int32 moveVal = MiniMax(Board, 0, false);

			// Metto la current tile occupata
			CurrTile->SetTileStatus(1, ETileStatus::OCCUPIED);

			if (!Board.Pieces.Contains(CurrTile->GetGridPosition()))
				Board.Pieces.Add(CurrTile->GetGridPosition(), Piece);

			Marked.Tile->SetTileStatus(-1, ETileStatus::EMPTY);
			Board.Pieces.Remove(Piece->GetGridPosition());
			Piece->SetGridPosition(StartPosition.X, StartPosition.Y);

			// Se ho catturato metto la tile occupata dallo human, altrimenti la mettoì empty
			if (PieceToCapture)
			{
				Board.CapturePieces.Remove(PieceToCapture);
				if (!Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
					Board.Pieces.Add(Marked.Tile->GetGridPosition(), PieceToCapture);
				Marked.Tile->SetTileStatus(0, ETileStatus::OCCUPIED);
			}

			if (moveVal > bestVal)
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
	GameInstance->SetTurnMessage(TEXT("AI Wins!"));
}

void AChess_Minimax::OnLose()
{
	GameInstance->SetTurnMessage(TEXT("AI Loses!"));
}
