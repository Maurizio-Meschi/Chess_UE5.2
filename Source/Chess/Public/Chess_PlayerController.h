// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Chess_HumanPlayer.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Chess_PlayerController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEventDelegate);
/**
 * 
 */
UCLASS()
class CHESS_API AChess_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AChess_PlayerController();

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* ChessContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* ClickAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player, HUD and UI")
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player, HUD and UI")
	UUserWidget* InventoryWidget;

	UPROPERTY(BlueprintAssignable, Category = "Events for HUD")
	FEventDelegate Event;

	void AddInventoryWidgetToViewport();

	void RemoveInventoryWidgetToViewport();

	void ClickOnGrid();

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void SetupInputComponent() override;
};