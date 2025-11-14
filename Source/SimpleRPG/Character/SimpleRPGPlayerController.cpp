// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRPGPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "../UI/SimpleRPGHUDWidget.h"


void ASimpleRPGPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Input setup for UI
	if (UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EIC->BindAction(InventoryToggleAction, ETriggerEvent::Triggered, this, &ASimpleRPGPlayerController::ToggleInventory);

		if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				UE_LOG(LogTemp, Error, TEXT("PlayerController UI Input added"));
				InputSystem->AddMappingContext(UIMapping, 1);
			}
		}
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
}

void ASimpleRPGPlayerController::AddPitchInput(float Val)
{
    Super::AddPitchInput(Val);
    
}

void ASimpleRPGPlayerController::ToggleInventory()
{
	UE_LOG(LogTemp, Log, TEXT("Inventory Toggle activation"));

	if (HUDWidget)
	{
		HUDWidget->ToggleInventory();
	}
}
