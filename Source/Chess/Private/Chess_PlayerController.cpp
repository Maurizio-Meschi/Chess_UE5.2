// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_PlayerController.h"
#include "Components/InputComponent.h"
#include "UObject/SoftObjectPtr.h"
#include "UMG.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "PawnPromotion.h"

AChess_PlayerController::AChess_PlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> InventoryWidgetBPClass(TEXT("/Game/Blueprints/HUD_PawnPromotion"));

	if (InventoryWidgetBPClass.Succeeded())
	{
		InventoryWidgetClass = InventoryWidgetBPClass.Class; 
	}
}

void AChess_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(ChessContext, 0);
	}

	// Only create the UI on the local machine (dose not excist on the server.)
	if (IsLocalPlayerController())
	{
		if (InventoryWidgetClass)
		{
			if (!InventoryWidget) // If the widget is not created and == NULL
			{
				InventoryWidget = CreateWidget<UUserWidget>(this, InventoryWidgetClass); // Create Widget
				if (!InventoryWidget)
					return;
				InventoryWidget->AddToViewport(); // Add it to the viewport so the Construct() method in the UUserWidget:: is run.
				InventoryWidget->SetVisibility(ESlateVisibility::Hidden); // Set it to hidden so its not open on spawn.
			}
		}
	}
}

void AChess_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Triggered, this, &AChess_PlayerController::ClickOnGrid);
	}
}

void AChess_PlayerController::ClickOnGrid()
{
	UE_LOG(LogTemp, Warning, TEXT("Click in controller!"));
	const auto HumanPlayer = Cast<AChess_HumanPlayer>(GetPawn());
	if (IsValid(HumanPlayer))
	{
		HumanPlayer->OnClick();
	}
}