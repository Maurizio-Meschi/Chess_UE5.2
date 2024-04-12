// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/MainActor/ChessPieces.h"
#include "Pieces/ChessPawn.h"
#include "../Public/MainActor/GameField.h"
#include "../Public/MainActor/Tile.h"
#include "../Public/Manager/Chess_GameMode.h"

// Sets default values
AChessPieces::AChessPieces()
{

	PrimaryActorTick.bCanEverTick = false;

	PieceGridPosition = FVector2D(0, 0);

	Color = EPieceColor::WHITE;
}

void AChessPieces::BeginPlay()
{
	Super::BeginPlay();
}

/*
*  @param: x,y value
*  @return: true if x,y belong to the board, false otherwise
*  @note: none
*/
bool AChessPieces::CheckCoord(int32 x, int32 y)
{
	if (x < 0 || x > 7 || y < 0 || y > 7) 
		return false;

	return true;
}

void AChessPieces::SetGridPosition(const double InX, const double InY)
{
	PieceGridPosition.Set(InX, InY);
}

void AChessPieces::SetColor(EPieceColor color)
{
	Color = color;
}

/*
*  @param: 1.) Tile where the piece actually is
*          2.) Tile where the movement was simulated
*          3.) Player
*          4.) IsTileEmpty: true if the move was simulated on an empty tile, false otherwise
* 
*  @return: none
*  @note: none
*/
void AChessPieces::ResetTileStatus(ATile* CurrTile, ATile* NewTile, int32 PlayerNumber, bool IsTileEmpty)
{
	if (IsTileEmpty)
	{
		CurrTile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);
		NewTile->SetTileStatus(-1, ETileStatus::EMPTY);

	}
	else
	{
		CurrTile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);
		NewTile->SetTileStatus(PlayerNumber == Player::Player1 ? Player::AI : Player::Player1, ETileStatus::OCCUPIED);
	}
	NewTile->SetVirtualStatus(EVirtualOccupied::VIRTUAL_EMPTY);
}


/*
*  @param: 1-2.) x,y where to simulate the move
*          3.)   Player
*          4.)   Boolean taken as reference: if it is set to true the calculation of legalmoves stops
*
*  @return: none
* 
*  @note: the function takes care of simulating the movement of the piece in the given position 
*         and checks whether this move is legal, i.e. whether the play does not expose the king to check
*/
void AChessPieces::MarkTile(FBoard& Board, int32 x, int32 y, int32 PlayerNumber, bool& Marked)
{
	AChess_GameMode* GMode = nullptr;
	AGameField* GField = nullptr;
	AManagePiece* ManagerPiece = nullptr;

	if (!FGameRef::GetGameRef(this, GMode, GField, ManagerPiece, "ChessPieces"))
		return;

	ATile* SelectedTile = nullptr;
	ATile* CurrTile = nullptr;

	// Takes the tile in x,y position
	if (Board.Field.Contains(FVector2D(x, y)))
		SelectedTile = Board.Field[FVector2D(x, y)];

	// Takes the tile where the piece is located
	if (Board.Field.Contains(GetGridPosition()))
		CurrTile = Board.Field[GetGridPosition()];


	if (SelectedTile && CurrTile)
	{
		/// Handle the case where the piece moves to an empty tile
		if (SelectedTile->GetTileStatus() == ETileStatus::EMPTY)
		{
			// If it is a pawn and if it is a tile where the pawn can only move by capturing, do nothing
			if (this->IsA<AChessPawn>() && Cast<AChessPawn>(this)->CaptureSituation)
				return;

			// Simulate the move
			SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);
			SelectedTile->SetVirtualStatus(this->IsA<AKing>() ? EVirtualOccupied::VIRTUAL_OCCUPIED_BY_KING : EVirtualOccupied::VIRTUAL_OCCUPIED);
			CurrTile->SetTileStatus(-1, ETileStatus::EMPTY);

			// Check if the move is legal
			auto Pieces = (PlayerNumber == Player::AI ? GField->GetPlayer1Pieces() : GField->GetPlayer2Pieces());
			for (auto EnemyPiece : Pieces)
			{
				if (!Board.CapturedPieces.Contains(EnemyPiece))
				{
					if (EnemyPiece->LegalMove(Board, PlayerNumber == Player::Player1 ? Player::AI : Player::Player1, true))
					{
						ResetTileStatus(CurrTile, SelectedTile, PlayerNumber, true);
						return;
					}
				}
			}
			// If the move is legal add it to legal move array
			ResetTileStatus(CurrTile, SelectedTile, PlayerNumber, true);
			FMarked Obj;
			Obj.Tile = SelectedTile;
			Obj.Capture = false;
			ManagerPiece->LegalMoveArray[IndexArray].Add(Obj);

		}
		/// Handle the case where the piece moves to an enemy tile
		else if (SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
		{
			// If it is a pawn and if it is a tile where the pawn cannot capture, do nothing
			if (this->IsA<AChessPawn>() && !Cast<AChessPawn>(this)->CaptureSituation)
			{
				Marked = true;
				return;
			}

			// Handle the fact that the king cannot be captured
			if (Board.Pieces.Contains(SelectedTile->GetGridPosition()))
			{
				auto Piece = Board.Pieces[SelectedTile->GetGridPosition()];
				if (Piece->IsA<AKing>())
				{
					Marked = true;
					return;
				}
			}

			// If the tile is occupied by an enemy piece, simulate capture
			if (SelectedTile->GetOwner() != PlayerNumber)
			{
				SelectedTile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);
				SelectedTile->SetVirtualStatus(this->IsA<AKing>() ? EVirtualOccupied::VIRTUAL_OCCUPIED_BY_KING : EVirtualOccupied::VIRTUAL_OCCUPIED);
				CurrTile->SetTileStatus(-1, ETileStatus::EMPTY);

				// Check if the move is legal
				auto Pieces = (PlayerNumber == Player::AI ? GField->GetPlayer1Pieces() : GField->GetPlayer2Pieces());
				for (auto EnemyPiece : Pieces)
				{
					if (!Board.CapturedPieces.Contains(EnemyPiece))
					{
						if (EnemyPiece->LegalMove(Board, PlayerNumber == Player::Player1 ? Player::AI : Player::Player1, true))
						{
							ResetTileStatus(CurrTile, SelectedTile, PlayerNumber, false);
							Marked = true;
							return;
						}
					}
				}
				// If the move is legal add it to legal move array
				ResetTileStatus(CurrTile, SelectedTile, PlayerNumber, false);
				FMarked Obj;
				Obj.Tile = SelectedTile;
				Obj.Capture = true;
				ManagerPiece->LegalMoveArray[this->IndexArray].Insert(Obj, 0);
			}
			Marked = true;
		}
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Error in select tile in ChessPieces"));

}

/*
* @param: 1.) Reference to the chessboard
*         2.) TilePosition: tile position where castling occurs
*         3.) Rook position
*         4.) Player
*         5.) Boolean taken as reference: if it is set to true the calculation of legalmoves stops
*
*  @return: none
* 
*  @note: Check if castling is possible
*/
void AChessPieces::Castling(FBoard& Board, FVector2D TilePosition, FVector2D RookPosition, int32 PlayerNumber, bool& Marked)
{
	AManagePiece* ManagerPiece = nullptr;

	if (!FGameRef::GetManagePiece(this, ManagerPiece, "ChessPieces"))
		return;

	// Tile to move the king to
	ATile* NextTile = nullptr;
	if (Board.Field.Contains(TilePosition))
		NextTile = Board.Field[TilePosition];

	// If tile is occupied do nothing
	if (NextTile && NextTile->GetTileStatus() == ETileStatus::EMPTY)
	{
		AChessPieces* Rook = nullptr;
		if (Board.Pieces.Contains(RookPosition))
		{
			Rook = Board.Pieces[RookPosition];
			if (Rook->IsA<ARook>() && Cast<ARook>(Rook)->NeverMoved)
			{
				// Call MarkTile and if the array of legal moves contains the tile then castling is possible
				MarkTile(Board, TilePosition.X, TilePosition.Y, PlayerNumber, Marked);
				auto TileMarked = ManagerPiece->LegalMoveArray[IndexArray];
				for (auto Element : TileMarked)
				{
					if (Element.Tile->GetGridPosition() == TilePosition)
						AManagePiece::Castling = true;
				}
			}
		}
	}
}


/*
* @param: 1.)   Reference to the chessboard
*         2-3.) x,y to check if the position contains the king
*         4.)   Palyer
*         5.)   Boolean taken as reference: if it is set to true the calculation of legalmoves stops
*
*  @return: true if the king is in check, false otherwise
*
*  @note: checks if the king is in check or if a simulated move has generated a check situation
*/
bool AChessPieces::TestCheck(FBoard& Board, int32 x, int32 y, int32 PlayerNumber, bool& Marked)
{
	AGameField* GField = nullptr;

	if (!FGameRef::GetGameField(this, GField, "ChessPieces"))
		return false;

	ATile* SelectedTile = nullptr;

	/*
	* I select the tile where the piece is located. 
	* If the piece is in a VIRTUAL_OCCUPIED tile it is as if it has been captured 
	* and I don't have to consider it
	*/
	if (Board.Field.Contains(GetGridPosition()))
		SelectedTile = Board.Field[GetGridPosition()];

	if (SelectedTile && SelectedTile->GetVirtaulStatus() == EVirtualOccupied::VIRTUAL_OCCUPIED)
	{
		Marked = true;
		return false;
	}

	SelectedTile = nullptr;

	// Select the tile at x,y position
	if (Board.Field.Contains(FVector2D(x, y)))
		SelectedTile = Board.Field[FVector2D(x, y)];

	if (SelectedTile)
	{
		// If the tile is empty do nothing
		if (SelectedTile->GetTileStatus() == ETileStatus::EMPTY)
		{
			return false;
		}
		else if (SelectedTile->GetTileStatus() == ETileStatus::OCCUPIED)
		{
			// If it is a pawn and if it is a tile where the pawn cannot capture, do nothing
			if (this->IsA<AChessPawn>() && !Cast<AChessPawn>(this)->CaptureSituation)
				return false;
			
			// If the king's movement was simulated 
			// and an enemy piece could move to that tile, the king would be in check
			if (SelectedTile->GetVirtaulStatus() == EVirtualOccupied::VIRTUAL_OCCUPIED_BY_KING)
			{
				return true;
			}
			// if the movement of a piece has been simulated 
			// and an enemy piece can go in that tile, there is no possibility of check in that direction
			else if (SelectedTile->GetVirtaulStatus() == EVirtualOccupied::VIRTUAL_OCCUPIED)
			{
				Marked = true;
				return false;
			}
			else
			{
				AChessPieces* Piece = nullptr;
				if (Board.Pieces.Contains(SelectedTile->GetGridPosition()))
					Piece = Board.Pieces[SelectedTile->GetGridPosition()];

				if (!Piece)
				{
					UE_LOG(LogTemp, Error, TEXT("Piece null in CheckTest"));
					return false;
				}

				// if the piece can attack the king it is check
				if (Piece->IsA<AKing>() && SelectedTile->GetOwner() != PlayerNumber)
				{
					return true;
				}
				else
					Marked = true;
			}
		}
		return false;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Selected tile null in ChessPieces"));
		return false;
	}
}