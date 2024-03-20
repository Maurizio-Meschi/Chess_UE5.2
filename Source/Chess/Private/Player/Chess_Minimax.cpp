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

		}, 0.1, false);
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
		if (Piece->Color == (Board.IsMax ? EPieceColor::WHITE : EPieceColor::BLACK))
			Score -= Piece->Value;
		else
			Score += Piece->Value;
		//UE_LOG(LogTemp, Error, TEXT("Score = %d"), Score);
	}

	// 3) Devo capire quante pedine sono adiacenti al re per valutare la sua protezione
	
	auto KingPosition = (Board.IsMax ? GField->GetKingArray()[Player::HUMAN]->GetGridPosition() : GField->GetKingArray()[Player::AI]->GetGridPosition());

	auto PiecesArray = (Board.IsMax ? GField->GetBotPieces() : GField->GetHumanPlayerPieces());
	for (auto Piece : PiecesArray)
	{
		if (!Board.CapturedPieces.Contains(Piece))
			Piece->LegalMove(Board, Board.IsMax ? Player::AI : Player::HUMAN, false);

		if (FindTileBetweenP1P2(Piece, Piece->GetGridPosition(), KingPosition, Board))
		{
			//UE_LOG(LogTemp, Error, TEXT("Pezzo: %s è arrivato a picchiare il re in %f %f"), *Piece->GetName(), KingPosition.X, KingPosition.Y);
			//break;
			Score += (Board.IsMax ? 7 : -7);
			//UE_LOG(LogTemp, Error, TEXT("Piece = %s ha messo sotto sacco e lo score vale: %d"), *Board.PieceMove->GetName(), Score);
		}
		
	}
	
	// 2) Devo considerare il numero di legal move disponibili per aumentare il valore 
	//    (Se il nemico ha poche legal move probabilmente è sotto scacco -> valore alto). Se io ho tante legalmove anche valore alto 
	//     Posso inglobarlo nel controllo se è checkmate (zero legal move nemico -> valore 10000, poche legal move nemico -> x punti etc) 
	
	PiecesArray = (Board.IsMax ? GField->GetBotPieces() : GField->GetHumanPlayerPieces());
	for (auto Piece : PiecesArray)
	{
		if (!Board.CapturedPieces.Contains(Piece))
			Piece->LegalMove(Board, Board.IsMax ? Player::AI : Player::HUMAN, false);
	}

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
		Score += (Board.IsMax ? 6 : -6);

	else if (MoveCount > 12)
		Score += (Board.IsMax ? 10 : -10);
	

	PiecesArray = (!Board.IsMax ? GField->GetBotPieces() : GField->GetHumanPlayerPieces());
	for (auto Piece : PiecesArray)
	{
		if (!Board.CapturedPieces.Contains(Piece))
			Piece->LegalMove(Board, !Board.IsMax ? Player::AI : Player::HUMAN, false);
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
		return (Board.IsMax ? 10000 : -10000);
	
	return Score;
}

bool AChess_Minimax::FindTileBetweenP1P2(AChessPieces* Piece, const FVector2D& P1, const FVector2D& P2, FBoard& Board)
{
	AChess_GameMode* GMode = nullptr;
	AGameField* GField = nullptr;
	AManagePiece* PieceManager = nullptr;
	if (!FGameRef::GetGameRef(this, GMode, GField, PieceManager, "Minimax") || !Piece)
		return false;

	auto TileMarked = PieceManager->TileMarkedForPiece[Piece->IndexArray];

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

			FMarked Obj;
			if (Board.Field.Contains(FVector2D(x, y)))
			{
				Obj.Tile = Board.Field[FVector2D(x, y)];
				
				if (Obj.Tile->GetTileStatus() == ETileStatus::OCCUPIED || !TileMarked.Contains(Obj))//cont == TileMarked.Num())
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

	if (Board.Field.Contains(P2))
	{
		FMarked Obj;
		Obj.Tile = Board.Field[P2];
		if (!TileMarked.Contains(Obj))
			return false;

		if (TileMarked.Contains(Obj) && Piece->IsA<AChessPawn>())
		{
			for (auto Marked : TileMarked)
			{
				if (Marked == Obj && !Marked.Capture)
					return false;
			}
		}
	}
	
	return true;
}

int32 AChess_Minimax::MiniMax(FBoard& Board, int32 Depth, int32 alpha, int32 beta, bool IsMax)
{
	int Score = EvaluateGrid(Board);

	//UE_LOG(LogTemp, Error, TEXT("Inizio il min max"));
	if (Depth == 2)
		return Score;

	if (Score == 10000 || Score == -10000)
		return Score;

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

		auto PlayerArray = GField->GetBotPieces();
	

		for (auto Piece : PlayerArray)
		{
			PieceManager->TileMarkedForPiece[Piece->IndexArray].Empty();

			if (!Board.CapturedPieces.Contains(Piece))
				Piece->LegalMove(Board, Player::AI, false);

			auto TileMarked = PieceManager->TileMarkedForPiece[Piece->IndexArray];

			// Se il pezzo non ha legal move oppure è stato catturato vado al prossimo pezzo
			if (TileMarked.Num() == 0 || Board.CapturedPieces.Contains(Piece))
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
						Board.CapturedPieces.Add(PieceToCapture);
						Board.Pieces.Remove(PieceToCapture->GetGridPosition());
					}
					else
						UE_LOG(LogTemp, Error, TEXT("PieceToCapture null in max"));
				}

				// Metto occupata dal bot la tile in cui mi sposto e sposto il pezzo
				Marked.Tile->SetTileStatus(Player::AI, ETileStatus::OCCUPIED);

				Board.Pieces.Add(Marked.Tile->GetGridPosition(), Piece);
				Piece->SetGridPosition(Marked.Tile->GetGridPosition().X, Marked.Tile->GetGridPosition().Y);

				//Board.PieceMove = Piece;
				best = FMath::Max(best, MiniMax(Board, Depth + 1, alpha, beta, !IsMax));

				// Metto la current tile occupata
				CurrTile->SetTileStatus(Player::AI, ETileStatus::OCCUPIED);

				Board.Pieces.Add(CurrTile->GetGridPosition(), Piece);

				Marked.Tile->SetTileStatus(-1, ETileStatus::EMPTY);
				Board.Pieces.Remove(Marked.Tile->GetGridPosition());
				Piece->SetGridPosition(CurrTile->GetGridPosition().X, CurrTile->GetGridPosition().Y);

				// Se ho catturato metto la tile occupata dallo human, altrimenti la mettoì empty
				if (PieceToCapture)
				{
					Board.CapturedPieces.Remove(PieceToCapture);
					Board.Pieces.Add(PieceToCapture->GetGridPosition(), PieceToCapture);
					Marked.Tile->SetTileStatus(Player::HUMAN, ETileStatus::OCCUPIED);
				}
				
				alpha = FMath::Max(alpha, best);
				if (beta <= alpha)
				{
					//UE_LOG(LogTemp, Error, TEXT("Poto nel max"));
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

		//Board.PlayerArray = GField->GetHumanPlayerPieces();
		auto PlayerArray = GField->GetHumanPlayerPieces();

		for (auto Piece : PlayerArray)
		{
			PieceManager->TileMarkedForPiece[Piece->IndexArray].Empty();
			if (!Board.CapturedPieces.Contains(Piece))
				Piece->LegalMove(Board, Player::HUMAN, false);

			
			auto TileMarked = PieceManager->TileMarkedForPiece[Piece->IndexArray];
			// Se il pezzo non ha legal move oppure è stato catturato vado al prossimo pezzo
			if (TileMarked.Num() == 0)// || Board.CapturedPieces.Contains(Piece))
				continue;

			for (auto Marked : TileMarked)
			{
				// Gestisco la tile dove era il pezzo prima di muoverlo
				ATile* CurrTile = nullptr;

				if (Board.Field.Contains(Piece->GetGridPosition()))
					CurrTile = Board.Field[Piece->GetGridPosition()];

				// Metto empty la tile in cui si sposta il pezzo
				CurrTile->SetTileStatus(-1, ETileStatus::EMPTY);
				Board.Pieces.Remove(CurrTile->GetGridPosition());

				// Se la tile è occupata alloco il pezzo nell'array dei pezzi catturati
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

				// Metto occupata dallo human la tile in cui mi sposto e sposto il pezzo
				Marked.Tile->SetTileStatus(Player::HUMAN, ETileStatus::OCCUPIED);

				Board.Pieces.Add(Marked.Tile->GetGridPosition(), Piece);
				Piece->SetGridPosition(Marked.Tile->GetGridPosition().X, Marked.Tile->GetGridPosition().Y);
				
				//Board.PieceMove = Piece;
				best = FMath::Min(best, MiniMax(Board, Depth + 1, alpha, beta, !IsMax));

				// Metto la current tile occupata
				CurrTile->SetTileStatus(Player::HUMAN, ETileStatus::OCCUPIED);

				Board.Pieces.Add(CurrTile->GetGridPosition(), Piece);

				Marked.Tile->SetTileStatus(-1, ETileStatus::EMPTY);
				Board.Pieces.Remove(Marked.Tile->GetGridPosition());
				Piece->SetGridPosition(CurrTile->GetGridPosition().X, CurrTile->GetGridPosition().Y);

				// Se ho catturato metto la tile occupata dallo human, altrimenti la mettoì empty
				if (PieceToCapture)
				{
					Board.CapturedPieces.Remove(PieceToCapture);
					Board.Pieces.Add(Marked.Tile->GetGridPosition(), PieceToCapture);
					Marked.Tile->SetTileStatus(Player::AI, ETileStatus::OCCUPIED);
				}
				
				beta = FMath::Min(beta, best);
				if (beta <= alpha)
				{
					UE_LOG(LogTemp, Error, TEXT("Poto nel min"));
					break;
				}
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
	for (int32 i = 0; i < PieceManager->TileMarkedForPiece.Num(); i++)
		PieceManager->TileMarkedForPiece[i].Empty();

	auto PlayerArray = GField->GetBotPieces();

	for (auto Piece : PlayerArray)
	{
		PieceManager->TileMarkedForPiece[Piece->IndexArray].Empty();
		if (!Board.CapturedPieces.Contains(Piece))
			Piece->LegalMove(Board, Player::AI, false);

		auto TileMarked = PieceManager->TileMarkedForPiece[Piece->IndexArray];

		// Se il pezzo non ha legal move oppure è stato catturato vado al prossimo pezzo
		if (TileMarked.Num() == 0)
			continue;

		for (auto Marked : TileMarked)
		{
			// Gestisco la tile dove era il pezzo prima di muoverlo
			ATile* CurrTile = nullptr;

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
					Board.CapturedPieces.Add(PieceToCapture);
				}
				else
					UE_LOG(LogTemp, Error, TEXT("PieceToCapture null bestmove"));
			}

			// Metto occupata dal bot la tile in cui mi sposto e sposto il pezzo
			Marked.Tile->SetTileStatus(Player::AI, ETileStatus::OCCUPIED);

			Board.Pieces.Add(Marked.Tile->GetGridPosition(), Piece);
			Piece->SetGridPosition(Marked.Tile->GetGridPosition().X, Marked.Tile->GetGridPosition().Y);

			//Board.PieceMove = Piece;
			int32 moveVal = MiniMax(Board, 0, -1000, 1000, false);

			// Metto la current tile occupata
			CurrTile->SetTileStatus(Player::AI, ETileStatus::OCCUPIED);

			Board.Pieces.Add(CurrTile->GetGridPosition(), Piece);

			Marked.Tile->SetTileStatus(-1, ETileStatus::EMPTY);
			Board.Pieces.Remove(Marked.Tile->GetGridPosition());
			Piece->SetGridPosition(CurrTile->GetGridPosition().X, CurrTile->GetGridPosition().Y);

			// Se ho catturato metto la tile occupata dallo human, altrimenti la mettoì empty
			if (PieceToCapture)
			{
				Board.CapturedPieces.Remove(PieceToCapture);
				Board.Pieces.Add(Marked.Tile->GetGridPosition(), PieceToCapture);
				Marked.Tile->SetTileStatus(Player::HUMAN, ETileStatus::OCCUPIED);
			}

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

void AChess_Minimax::OnWin()
{
	GameInstance->SetTurnMessage(TEXT("AI Wins!"));
}

void AChess_Minimax::OnLose()
{
	GameInstance->SetTurnMessage(TEXT("AI Loses!"));
}
