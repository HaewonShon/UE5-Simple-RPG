// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/UI/RootWidget.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

#include "Player/UI/SimpleRPGHUDWidget.h"
#include "Player/UI/Inventory/InventoryWidget.h"
#include "Shared/Item/UI/ItemDescriptionWidget.h"

UUserWidget* URootWidget::AddWidgetToLayer(EWidgetLayer Layer, TSubclassOf<UUserWidget> WidgetClass, bool bFillScreen)
{
	UOverlay* Overlay = GetLayer(Layer);
	check(Overlay);

	UUserWidget* Widget = CreateWidget<UUserWidget>(this, WidgetClass.Get());
	if (Widget)
	{
		UOverlaySlot* ChildSlot = Overlay->AddChildToOverlay(Widget);
		if (bFillScreen)
		{
			ChildSlot->SetHorizontalAlignment(HAlign_Fill);
			ChildSlot->SetVerticalAlignment(VAlign_Fill);
			ChildSlot->SetPadding(FMargin(0.f));
		}
		++LiveWidgetCountForLayer[Layer];
		UpdateBlockingImageStatus(Layer);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to add widget %s"), *(WidgetClass.Get()->GetFName().ToString()));
	}
	return Widget;
}

void URootWidget::ToggleInventory()
{
	if (InventoryWidget->GetVisibility() == ESlateVisibility::Collapsed)
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Visible);
		++LiveWidgetCountForLayer[EWidgetLayer::Menu];
	}
	else
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
		--LiveWidgetCountForLayer[EWidgetLayer::Menu];
	}
	UpdateBlockingImageStatus(EWidgetLayer::Menu);
}

void URootWidget::NotifyWidgetRemoved(EWidgetLayer Layer)
{
	--LiveWidgetCountForLayer[Layer];
	UpdateBlockingImageStatus(Layer);
}

void URootWidget::NativeConstruct()
{
	LiveWidgetCountForLayer.Add({ EWidgetLayer::HUD, 0 });
	LiveWidgetCountForLayer.Add({ EWidgetLayer::Dialogue, 0 });
	LiveWidgetCountForLayer.Add({ EWidgetLayer::Menu, 0 });
	LiveWidgetCountForLayer.Add({ EWidgetLayer::System, 0 });

	check(HUDWidgetClass);
	AddWidgetToLayer(EWidgetLayer::HUD, HUDWidgetClass, true);

	check(InventoryWidgetClass);
	InventoryWidget = Cast<UInventoryWidget>(AddWidgetToLayer(EWidgetLayer::Menu, InventoryWidgetClass, true));
	ToggleInventory();

	check(ItemDescriptionWidgetClass);
	ItemDescriptionWidget = Cast<UItemDescriptionWidget>(AddWidgetToLayer(EWidgetLayer::System, ItemDescriptionWidgetClass));
	
	InventoryWidget->SetDescriptionWidgetRef(ItemDescriptionWidget);
}

void URootWidget::UpdateBlockingImageStatus(EWidgetLayer Layer)
{
	check(LiveWidgetCountForLayer[Layer] >= 0);
	if (LiveWidgetCountForLayer[Layer] > 0)
	{
		// turn on blocking image for layer
	}
	else
	{
		// turn off blocking image for layer
	}
}

UOverlay* URootWidget::GetLayer(EWidgetLayer Layer)
{
	switch (Layer)
	{
	case EWidgetLayer::HUD:
		return HUDLayer;
	case EWidgetLayer::Dialogue:
		return DialogueLayer;
	case EWidgetLayer::Menu:
		return MenuLayer;
	case EWidgetLayer::System:
		return SystemLayer;
	}
	return nullptr;
}
