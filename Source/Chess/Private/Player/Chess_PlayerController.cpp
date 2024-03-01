// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/Player/Chess_PlayerController.h"
#include "Components/InputComponent.h"
#include "UObject/SoftObjectPtr.h"
#include "UMG.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"


AChess_PlayerController::AChess_PlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true; 
}

void AChess_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(ChessContext, 0);
	}

	
	if (IsLocalPlayerController())
	{
		if (InventoryWidgetClass)
		{
			if (!InventoryWidget)
			{
				InventoryWidget = CreateWidget<UUserWidget>(this, InventoryWidgetClass);
			}
		}
		else
			UE_LOG(LogTemp, Error, TEXT("No BP found in controller"));
	}
}

void AChess_PlayerController::BeginDestroy()
{
	Super::BeginDestroy();

	if (InventoryWidget && InventoryWidget->IsInViewport())
	{
		InventoryWidget->RemoveFromParent();
	}

	//InventoryWidget = nullptr;
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

void AChess_PlayerController::AddInventoryWidgetToViewport()
{
	if (!InventoryWidget)
	{
		if (InventoryWidgetClass)
		{
				InventoryWidget = CreateWidget<UUserWidget>(this, InventoryWidgetClass);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("No BP found in controller"));
	}
	if (!InventoryWidget->IsInViewport())
		InventoryWidget->AddToViewport();
	/*
	if (InventoryWidget && InventoryWidget->IsInViewport())
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Visible);
		InventoryWidget->SetIsEnabled(true);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Widget null in controller"));
		*/
}

void AChess_PlayerController::RemoveInventoryWidgetToViewport()
{
	if (!InventoryWidget)
	{
		if (InventoryWidgetClass)
		{
			InventoryWidget = CreateWidget<UUserWidget>(this, InventoryWidgetClass);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("No BP found in controller"));
	}
	if (InventoryWidget && InventoryWidget->IsInViewport())
		InventoryWidget->RemoveFromParent();
	/*
	if (InventoryWidget && InventoryWidget->IsInViewport())
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
		InventoryWidget->SetIsEnabled(false);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Widget null in controller"));
		*/
}

void AChess_PlayerController::ClickOnGrid()
{
	//UE_LOG(LogTemp, Warning, TEXT("Click in controller!"));
	const auto HumanPlayer = Cast<AChess_HumanPlayer>(GetPawn());
	if (IsValid(HumanPlayer))
	{
		HumanPlayer->OnClick();
	}
}