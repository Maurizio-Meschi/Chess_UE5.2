// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_PlayerController.h"
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

UUserWidget* AChess_PlayerController::InventoryWidget = nullptr;

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
				InventoryWidget->AddToViewport();
				InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
				InventoryWidget->SetIsEnabled(false);
			}
		}
		else
			UE_LOG(LogTemp, Error, TEXT("No BP found in controller"));
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

void AChess_PlayerController::AddInventoryWidgetToViewport()
{
	if (InventoryWidget && InventoryWidget->IsInViewport())
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Visible);
		InventoryWidget->SetIsEnabled(true);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Widget null in controller"));
}

void AChess_PlayerController::RemoveInventoryWidgetToViewport()
{
	if (InventoryWidget && InventoryWidget->IsInViewport())
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
		InventoryWidget->SetIsEnabled(false);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Widget null in controller"));
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

AChess_PlayerController* FControllerRef::CachedController = nullptr;

AChess_PlayerController* FControllerRef::GetController(UObject* WorldContextObject)
{
	if (!CachedController)
	{
		UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
		if (World)
		{
			AChess_PlayerController* PlayerController = Cast<AChess_PlayerController>(UGameplayStatics::GetPlayerController(World, 0));;
			CachedController = Cast<AChess_PlayerController>(PlayerController);
		}
	}

	return CachedController;
}
