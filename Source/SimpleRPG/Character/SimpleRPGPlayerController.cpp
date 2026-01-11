// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRPGPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "../UI/SimpleRPGHUDWidget.h"


void ASimpleRPGPlayerController::BeginPlay()
{
    Super::BeginPlay();

	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	check(LocalPlayer);
	UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(InputSystem)

	// Input setup for UI
	if (UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EIC->BindAction(InventoryToggleAction, ETriggerEvent::Triggered, this, &ASimpleRPGPlayerController::ToggleInventory);
		InputSystem->AddMappingContext(UIMapping, 1);
		
#if !UE_BUILD_SHIPPING
		ensure(CheatAction.Num() >= 4);
		EIC->BindAction(CheatAction[0], ETriggerEvent::Triggered, this, &ASimpleRPGPlayerController::CheatFunction1);
		EIC->BindAction(CheatAction[1], ETriggerEvent::Triggered, this, &ASimpleRPGPlayerController::CheatFunction2);
		EIC->BindAction(CheatAction[2], ETriggerEvent::Triggered, this, &ASimpleRPGPlayerController::CheatFunction3);
		EIC->BindAction(CheatAction[3], ETriggerEvent::Triggered, this, &ASimpleRPGPlayerController::CheatFunction4);
		InputSystem->AddMappingContext(CheatMapping, 0);
#endif

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController requires EnhancedInputComponent"));
	}

    if (HUDWidgetClass)
    {
        HUDWidget = CreateWidget<USimpleRPGHUDWidget>(this, HUDWidgetClass);
        if (HUDWidget)
        {
			HUDWidget->AddToViewport();
        }
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create HUD"));
		}
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("HUDWidget in PlayerController not registered."));
    }

	bIsInvenetoryOn = false;
}

void ASimpleRPGPlayerController::AddPitchInput(float Val)
{
    Super::AddPitchInput(Val);
    
}

void ASimpleRPGPlayerController::ToggleInventory()
{
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (HUDWidget)
	{
		HUDWidget->ToggleInventory();
		bIsInvenetoryOn = !bIsInvenetoryOn;

		if(bIsInvenetoryOn)
		{
			//InputSystem->RemoveMappingContext(GameInputMaapping);

			bShowMouseCursor = true;

			FInputModeGameAndUI InputMode;
			InputMode.SetHideCursorDuringCapture(false);
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputMode.SetWidgetToFocus(HUDWidget->GetInventoryWidget()->TakeWidget());

			SetInputMode(InputMode);
		}
		else
		{
			//InputSystem->AddMappingContext(GameInputMaapping, 0);

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}
	}
}

#if !UE_BUILD_SHIPPING
#include "../ItemTestCheatManager.h"
#include "../Quest/QuestTestCheatManager.h"
void ASimpleRPGPlayerController::CheatFunction1()
{
	if (UItemTestCheatManager* ItemCheatManager = Cast<UItemTestCheatManager>(CheatManager))
	{
		ItemCheatManager->GiveItem(0);
	}
	else if (UQuestTestCheatManager* QuestCheatManager = Cast<UQuestTestCheatManager>(CheatManager))
	{
		QuestCheatManager->GiveQuest(0);
	}
}

void ASimpleRPGPlayerController::CheatFunction2()
{
	if (UItemTestCheatManager* ItemCheatManager = Cast<UItemTestCheatManager>(CheatManager))
	{
		ItemCheatManager->GiveItem(1);
	}
	else if (UQuestTestCheatManager* QuestCheatManager = Cast<UQuestTestCheatManager>(CheatManager))
	{
		QuestCheatManager->GiveQuest(1);
	}
}

void ASimpleRPGPlayerController::CheatFunction3()
{
	if (UItemTestCheatManager* ItemCheatManager = Cast<UItemTestCheatManager>(CheatManager))
	{
		ItemCheatManager->GiveItem(2);
	}
	else if (UQuestTestCheatManager* QuestCheatManager = Cast<UQuestTestCheatManager>(CheatManager))
	{
		QuestCheatManager->GiveQuest(2);
	}
}

void ASimpleRPGPlayerController::CheatFunction4()
{
	if (UItemTestCheatManager* ItemCheatManager = Cast<UItemTestCheatManager>(CheatManager))
	{
		ItemCheatManager->GiveItem(3);
	}
	else if (UQuestTestCheatManager* QuestCheatManager = Cast<UQuestTestCheatManager>(CheatManager))
	{
		QuestCheatManager->GiveQuest(3);
	}
}
#endif