// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/UI/UISubsystem.h"
#include "UISubsystem.h"

void UUISubsystem::InitializeRootWidget(APlayerController* PC, TSubclassOf<class URootWidget> RootWidgetClass)
{
	RootWidget = CreateWidget<URootWidget>(PC, RootWidgetClass.Get());
	RootWidget->AddToViewport();
}

UUserWidget* UUISubsystem::AddWidgetToLayer(EWidgetLayer Layer, TSubclassOf<class UUserWidget> WidgetClass, bool bFillScreen, bool bIsAliveAways)
{
	return RootWidget->AddWidgetToLayer(Layer, WidgetClass, bFillScreen, bIsAliveAways);
}

void UUISubsystem::ToggleInventory()
{
	RootWidget->ToggleInventory();
}
