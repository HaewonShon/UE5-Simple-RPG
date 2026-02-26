// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/UI/RootWidget.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/Image.h"

#include "Player/UI/SimpleRPGHUDWidget.h"
#include "Player/UI/Inventory/InventoryWidget.h"
#include "Shared/Item/UI/ItemDescriptionWidget.h"
#include "Shared/UI/Common/SessionWidget.h"

UUserWidget* URootWidget::AddWidgetToLayer(EWidgetLayer Layer, TSubclassOf<UUserWidget> WidgetClass, bool bFillScreen, bool bIsAliveAlways)
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
		else
		{
			ChildSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
			ChildSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
			ChildSlot->SetPadding(FMargin(0.f));
		}

		if (!bIsAliveAlways)
		{
			if (USessionWidget* SessionWidget = Cast<USessionWidget>(Widget))
			{
				UE_LOG(LogTemp, Log, TEXT("Widget close event registered"));
				SessionWidget->OnWidgetClosed.AddUObject(this, &URootWidget::RequestRemoveWidget, Layer, Widget);
			}
			++LiveWidgetCountForLayer[Layer];
			UpdateBlockingImageStatus(Layer);
		}
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
	}
	else
	{
		InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void URootWidget::RequestRemoveWidget(EWidgetLayer Layer, UUserWidget* Widget)
{
	UE_LOG(LogTemp, Warning, TEXT("RequestRemoveWidget"));
	if (!Widget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed RequestRemoveWidget"));
		return;
	}

	Widget->RemoveFromParent();
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
	AddWidgetToLayer(EWidgetLayer::HUD, HUDWidgetClass, true, true);

	check(InventoryWidgetClass);
	InventoryWidget = Cast<UInventoryWidget>(AddWidgetToLayer(EWidgetLayer::Menu, InventoryWidgetClass, true, true));
	ToggleInventory();

	check(ItemDescriptionWidgetClass);
	ItemDescriptionWidget = Cast<UItemDescriptionWidget>(AddWidgetToLayer(EWidgetLayer::System, ItemDescriptionWidgetClass, false, true));
	ItemDescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);
	UOverlaySlot* DescriptionWidgetSlot = Cast<UOverlaySlot>(ItemDescriptionWidget->Slot);
	DescriptionWidgetSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Left);
	DescriptionWidgetSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Top);

	InventoryWidget->SetDescriptionWidgetRef(ItemDescriptionWidget);
	// shop->SetDescriptionWidgetRef
}

void URootWidget::UpdateBlockingImageStatus(EWidgetLayer Layer)
{
	check(LiveWidgetCountForLayer[Layer] >= 0);
	
	ESlateVisibility NewVisibility = (LiveWidgetCountForLayer[Layer] > 0) ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	switch (Layer)
	{
	case EWidgetLayer::Dialogue:
		DialogueLayerBlockingImage->SetVisibility(NewVisibility);
		break;
	case EWidgetLayer::Menu:
		MenuLayerBlockingImage->SetVisibility(NewVisibility);
		break;
	case EWidgetLayer::System:
		SystemLayerBlockingImage->SetVisibility(NewVisibility);
		break;
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
