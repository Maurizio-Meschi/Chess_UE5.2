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

	// Get widget reference
	if (IsLocalPlayerController())
	{
		if (WidgetPawnPromotionClass)
		{
			if (!WidgetPawnPromotion)
			{
				WidgetPawnPromotion = CreateWidget<UUserWidget>(this, WidgetPawnPromotionClass);
			}
		}
		else
			UE_LOG(LogTemp, Error, TEXT("No BP found in controller"));
	}
}

void AChess_PlayerController::BeginDestroy()
{
	Super::BeginDestroy();

	// if present remove the widget from the viewport when the object is destroyed
	if (WidgetPawnPromotion && WidgetPawnPromotion->IsInViewport())
	{
		WidgetPawnPromotion->RemoveFromParent();
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

/*
* @param: none
* @return: none
* @note: if there is no reference to the widget, 
*        it is taken and added to the viewport
*/
void AChess_PlayerController::AddWidgetPawnPromotionToViewport()
{
	if (!WidgetPawnPromotion)
	{
		if (WidgetPawnPromotionClass)
		{
				WidgetPawnPromotion = CreateWidget<UUserWidget>(this, WidgetPawnPromotionClass);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("No BP found in controller"));
	}
	if (!WidgetPawnPromotion->IsInViewport())
		WidgetPawnPromotion->AddToViewport();
}

/*
* @param: none
* @return: none
* @note: if there is no reference to the widget,
*        it is taken and removed to the viewport
*/
void AChess_PlayerController::RemoveWidgetPawnPromotionToViewport()
{
	if (!WidgetPawnPromotion)
	{
		if (WidgetPawnPromotionClass)
		{
			WidgetPawnPromotion = CreateWidget<UUserWidget>(this, WidgetPawnPromotionClass);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("No BP found in controller"));
	}
	if (WidgetPawnPromotion && WidgetPawnPromotion->IsInViewport())
		WidgetPawnPromotion->RemoveFromParent();
}

void AChess_PlayerController::ClickOnGrid()
{
	const auto HumanPlayer = Cast<AChess_HumanPlayer>(GetPawn());
	if (IsValid(HumanPlayer))
	{
		HumanPlayer->OnClick();
	}
}