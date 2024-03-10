// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

UENUM()
enum class ETileStatus : uint8
{
	EMPTY				UMETA(DisplayName = "Empty"),
	OCCUPIED			UMETA(DisplayName = "Occupied"),
	MARKED_TO_CAPTURE	UMETA(DisplayName = "Occupied by enemy piece"),
	MARKED				UMETA(DisplayNmae = "Marked"),
};

UENUM()
enum class EVirtualOccupied : uint8
{
	VIRTUAL_EMPTY			 UMETA(DisplayName = "Virtual Empty"),
	VIRTUAL_OCCUPIED		 UMETA(DisplayName = "Virtual Occupied by enemy piece"),
	VIRTUAL_OCCUPIED_BY_KING UMETA(DisplayName = "Virtual Occupied by King"),
};

UCLASS()
class CHESS_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

	// set the player owner and the status of a tile
	void SetTileStatus(const int32 TileOwner, const ETileStatus TileStatus);

	void SetVirtualStatus(const EVirtualOccupied TileStatus);

	// set the (x, y) position
	void SetGridPosition(const double InX, const double InY);

	// get the tile status
	ETileStatus GetTileStatus();

	EVirtualOccupied GetVirtaulStatus();

	// get the tile owner
	int32 GetOwner();

	// get the (x, y) position
	FVector2D GetGridPosition();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Name;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ETileStatus Status;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EVirtualOccupied VirtualStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 PlayerOwner;

	// (x, y) position of the tile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector2D TileGridPosition;
};
