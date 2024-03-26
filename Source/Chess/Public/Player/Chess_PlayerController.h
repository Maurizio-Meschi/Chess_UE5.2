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
	TSubclassOf<UUserWidget> WidgetPawnPromotionClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player, HUD and UI")
	UUserWidget* WidgetPawnPromotion;

	UPROPERTY(BlueprintAssignable, Category = "Events for HUD")
	FEventDelegate Event;

	// Add pawn promotion widget to viewport
	void AddWidgetPawnPromotionToViewport();

	// Remove pawn promotion widget to viewport
	void RemoveWidgetPawnPromotionToViewport();

	// Calls the human's on click function
	void ClickOnGrid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called when the game ends or the object is destroyed
	virtual void BeginDestroy() override;
	virtual void SetupInputComponent() override;
};