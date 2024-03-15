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

			TMap<FVector2D, ATile*> CopyTileMap;
			TMap<FVector2D, ATile*> TileMap = GField->GetTileMap();
			for (auto Element : TileMap)
				CopyTileMap.Add(Element);

			TMap<FVector2D, AChessPieces*> CopyPiecesMap;
			TMap<FVector2D, AChessPieces*> PiecesMap = GField->GetPiecesMap();
			for (auto Element : PiecesMap)
				CopyPiecesMap.Add(Element);


			FBoard Board;
			Board.Field = GField->GetTileMap();
			Board.Pieces = GField->GetPiecesMap();
			UE_LOG(LogTemp, Error, TEXT("Vado a cercare la best move"));

			FMarked TileToMove = FindBestMove(Board);

			GField->SetTileMap(CopyTileMap);
			GField->SetPiecesMap(CopyPiecesMap);

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

		}, 2, false);
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
			Score += Piece->Value;
		else
			Score -= Piece->Value;
	}
	// 2) Devo considerare il numero di legal move disponibili per aumentare il valore 
	//    (Se il nemico ha poche legal move probabilmente è sotto scacco -> valore alto). Se io ho tante legalmove anche valore alto 
	//     Posso inglobarlo nel controllo se è checkmate (zero legal move nemico -> valore 10000, poche legal move nemico -> x punti etc) 
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
	auto KingPosition = (Board.IsMax ? GField->GetKingArray()[0]->GetGridPosition() : GField->GetKingArray()[1]->GetGridPosition());
	auto PiecePosition = Board.NewPieceTile->GetGridPosition();

	// Se il pezzo che ho spostato mette sotto scacco il re è una buona giocata
	//if (FindTileBetweenP1P2(PiecePosition, KingPosition, Board))
		//Score += 10;

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
	UE_LOG(LogTemp, Error, TEXT("Inizio il min max"));
	if (Depth > 4)
		return -1000;

	AChess_GameMode* GMode = nullptr;
	AGameField* GField = nullptr;
	AManagePiece* PieceManager = nullptr;
	if (!FGameRef::GetGameRef(this, GMode, GField, PieceManager, "Minimax"))
		return 0;

	int score = EvaluateGrid(Board);

	auto TileMarkedForPiece = PieceManager->TileMarkedForPiece;

	// Pulisco tutte le legalMove
	for (int32 i = 0; i < TileMarkedForPiece.Num(); i++)
		TileMarkedForPiece[i].Empty();

	// If this maximizer's move
	if (IsMax)
	{
		int32 best = -1000;

		Board.IsMax = true;

		auto Pieces = Board.Pieces;
		for (auto Element : Pieces)
		{
			AChessPieces* Piece = Element.Value;

			// Devo lavorare solo sui pezzi neri
			if (Piece->Color == EPieceColor::WHITE)
				continue;

			Piece->LegalMove(GMode->CurrentPlayer, false);
		}

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

		for (auto Element : Pieces)
		{
			AChessPieces* Piece = Element.Value;

			// Devo lavorare solo sui pezzi neri
			if (Piece->Color == EPieceColor::WHITE)
				continue;

			auto TileMarked = PieceManager->TileMarkedForPiece[Piece->IndexArray];

			if (TileMarked.Num() == 0)
				continue;

			for (auto Marked : TileMarked)
			{
				// Gestisco la tile dove era il pezzo prima di muoverlo
				ATile* CurrTile = nullptr;

				if (Board.Field.Contains(Piece->GetGridPosition()))
					CurrTile = Board.Field[Piece->GetGridPosition()];

				// Metto empty la tile in cui si trovava il pezzo
				CurrTile->SetTileStatus(-1, ETileStatus::EMPTY);
				// Tolgo dalla mappa il pezzo in quella posizione
				Board.Pieces.Remove(CurrTile->GetGridPosition());

				// Se catturo levo il pezzo catturato dai pezzi
				AChessPieces* CapturePiece = nullptr;
				if (Marked.Capture)
				{
					if (Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
					{
						CapturePiece = Board.Pieces[Marked.Tile->GetGridPosition()];
						Board.Pieces.Remove(Marked.Tile->GetGridPosition());
					}
				}

				// Metto occuapata la tile in cui si sposta
				Marked.Tile->SetTileStatus(GMode->CurrentPlayer, ETileStatus::OCCUPIED);

				Board.NewPieceTile = Marked.Tile;

				// sposto il pezzo nella nuova tile
				if (!Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
					Board.Pieces.Add(Marked.Tile->GetGridPosition(), Piece);

				best = FMath::Max(best, MiniMax(Board, Depth + 1, !IsMax)); 

				// ripristino le strutture dati
				if (Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
					Board.Pieces.Remove(Marked.Tile->GetGridPosition());
				
				if (!Board.Pieces.Contains(CurrTile->GetGridPosition()))
					Board.Pieces.Add(CurrTile->GetGridPosition(), Piece);

				if (Marked.Capture)
				{
					if (!Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
					{
						Board.Pieces.Add(Marked.Tile->GetGridPosition(), CapturePiece);
						Marked.Tile->SetTileStatus(0, ETileStatus::OCCUPIED);
					}
				}
				else
					Marked.Tile->SetTileStatus(-1, ETileStatus::EMPTY);

				CurrTile->SetTileStatus(GMode->CurrentPlayer, ETileStatus::OCCUPIED);
			}
		}
		return best;
	}

	// If this minimizer's move
	else
	{
		int32 best = -1000;

		Board.IsMax = false;

		auto Pieces = Board.Pieces;
		for (auto Element : Pieces)
		{
			AChessPieces* Piece = Element.Value;

			// Devo lavorare solo sui pezzi bianchi
			if (Piece->Color == EPieceColor::BLACK)
				continue;

			Piece->LegalMove(GMode->CurrentPlayer, false);
		}

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

		for (auto Element : Pieces)
		{
			AChessPieces* Piece = Element.Value;

			// Devo lavorare solo sui pezzi bianchi
			if (Piece->Color == EPieceColor::BLACK)
				continue;

			auto TileMarked = PieceManager->TileMarkedForPiece[Piece->IndexArray];

			if (TileMarked.Num() == 0)
				continue;

			for (auto Marked : TileMarked)
			{
				// Gestisco la tile dove era il pezzo prima di muoverlo
				ATile* CurrTile = nullptr;

				if (Board.Field.Contains(Piece->GetGridPosition()))
					CurrTile = Board.Field[Piece->GetGridPosition()];

				// Metto empty la tile in cui si trovava il pezzo
				CurrTile->SetTileStatus(-1, ETileStatus::EMPTY);
				// Tolgo dalla mappa il pezzo in quella posizione
				Board.Pieces.Remove(CurrTile->GetGridPosition());

				// Se catturo levo il pezzo catturato dai pezzi
				AChessPieces* CapturePiece = nullptr;
				if (Marked.Capture)
				{
					if (Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
					{
						CapturePiece = Board.Pieces[Marked.Tile->GetGridPosition()];
						Board.Pieces.Remove(Marked.Tile->GetGridPosition());
					}
				}

				// Metto occuapata la tile in cui si sposta
				Marked.Tile->SetTileStatus(0, ETileStatus::OCCUPIED);

				Board.NewPieceTile = Marked.Tile;

				// sposto il pezzo nella nuova tile
				if (!Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
					Board.Pieces.Add(Marked.Tile->GetGridPosition(), Piece);

				best = FMath::Max(best, MiniMax(Board, Depth + 1, !IsMax));

				// ripristino le strutture dati
				if (Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
					Board.Pieces.Remove(Marked.Tile->GetGridPosition());

				if (!Board.Pieces.Contains(CurrTile->GetGridPosition()))
					Board.Pieces.Add(CurrTile->GetGridPosition(), Piece);

				if (Marked.Capture)
				{
					if (!Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
					{
						Board.Pieces.Add(Marked.Tile->GetGridPosition(), CapturePiece);
						Marked.Tile->SetTileStatus(1, ETileStatus::OCCUPIED);
					}
				}
				else
					Marked.Tile->SetTileStatus(-1, ETileStatus::EMPTY);

				CurrTile->SetTileStatus(0, ETileStatus::OCCUPIED);
			}
		}
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

	auto Pieces = Board.Pieces;
	for (auto Element : Pieces)
	{
		AChessPieces* Piece = Element.Value;

		// Devo lavorare solo sui pezzi neri
		if (Piece->Color == EPieceColor::WHITE)
			continue;

		Piece->LegalMove(GMode->CurrentPlayer, false);
	}

	for (auto Element : Pieces)
	{
		AChessPieces* Piece = Element.Value;

		// Devo lavorare solo sui pezzi neri
		if (Piece->Color == EPieceColor::WHITE)
			continue;

		auto TileMarked = PieceManager->TileMarkedForPiece[Piece->IndexArray];

		if (TileMarked.Num() == 0)
			continue;

		for (auto Marked : TileMarked)
		{
			// Gestisco la tile dove era il pezzo prima di muoverlo
			ATile* CurrTile = nullptr;

			if (Board.Field.Contains(Piece->GetGridPosition()))
				CurrTile = Board.Field[Piece->GetGridPosition()];

			// Metto empty la tile in cui si trovava il pezzo
			CurrTile->SetTileStatus(-1, ETileStatus::EMPTY);
			// Tolgo dalla mappa il pezzo in quella posizione
			Board.Pieces.Remove(CurrTile->GetGridPosition());

			// Se catturo levo il pezzo catturato dai pezzi
			AChessPieces* CapturePiece = nullptr;
			if (Marked.Capture)
			{
				if (Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
				{
					CapturePiece = Board.Pieces[Marked.Tile->GetGridPosition()];
					Board.Pieces.Remove(Marked.Tile->GetGridPosition());
				}
			}

			// Metto occuapata la tile in cui si sposta
			Marked.Tile->SetTileStatus(GMode->CurrentPlayer, ETileStatus::OCCUPIED);

			Board.NewPieceTile = Marked.Tile;

			// sposto il pezzo nella nuova tile
			if (!Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
				Board.Pieces.Add(Marked.Tile->GetGridPosition(), Piece);

			int32 moveVal = MiniMax(Board, 0, false);

			// ripristino le strutture dati
			if (Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
				Board.Pieces.Remove(Marked.Tile->GetGridPosition());

			if (!Board.Pieces.Contains(CurrTile->GetGridPosition()))
				Board.Pieces.Add(CurrTile->GetGridPosition(), Piece);

			if (Marked.Capture)
			{
				if (!Board.Pieces.Contains(Marked.Tile->GetGridPosition()))
				{
					Board.Pieces.Add(Marked.Tile->GetGridPosition(), CapturePiece);
					Marked.Tile->SetTileStatus(0, ETileStatus::OCCUPIED);
				}
			}
			else
				Marked.Tile->SetTileStatus(-1, ETileStatus::EMPTY);

			CurrTile->SetTileStatus(GMode->CurrentPlayer, ETileStatus::OCCUPIED);

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
