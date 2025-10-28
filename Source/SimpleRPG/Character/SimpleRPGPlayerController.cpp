// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRPGPlayerController.h"
#include "Blueprint/UserWidget.h"

void ASimpleRPGPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (HUDWidget)
    {
        UUserWidget* HUD = CreateWidget<UUserWidget>(this, HUDWidget.Get());
        if (HUD)
        {
            HUD->AddToViewport();
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
