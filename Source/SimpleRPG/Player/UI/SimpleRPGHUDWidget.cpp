// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRPGHUDWidget.h"
#include "Inventory/InventoryWidget.h"
#include "Inventory/BackdropWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Shared/Item/UI/ItemDescriptionWidget.h"

void USimpleRPGHUDWidget::NativeConstruct()
{ 
    InventoryWidget = CreateWidget<UInventoryWidget>(GetOwningPlayer(), InventoryWidgetClass.Get());
    if (InventoryWidget)
    {
        InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
        BackdropWidget->SetVisibility(ESlateVisibility::Collapsed);

        UCanvasPanelSlot* InventoryWidgetSlot = MainCanvas->AddChildToCanvas(InventoryWidget);
        const FVector2D InventoryWidgetSize{ 640.f, 900.f };

        InventoryWidgetSlot->SetAnchors(FAnchors(1.f, 0.f));
        InventoryWidgetSlot->SetAlignment(FVector2D(1.f, 0.f));
        InventoryWidgetSlot->SetOffsets(FMargin(-(20.f), 20.f, InventoryWidgetSize.X, InventoryWidgetSize.Y));

        InventoryWidget->BindItemDiscardDelegate(BackdropWidget.Get());

        ItemDescriptionWidget = CreateWidget<UItemDescriptionWidget>(GetOwningPlayer(), ItemDescriptionWidgetClass.Get());
        if (ItemDescriptionWidget)
        {
            ItemDescriptionWidget->AddToViewport(10); // magic number for z-order
            ItemDescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);
            InventoryWidget->SetDescriptionWidgetRef(ItemDescriptionWidget);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create Inventory Widget"));
    }
}

void USimpleRPGHUDWidget::ToggleInventory()
{
    ESlateVisibility CurrentInventoryVisilbity = InventoryWidget->GetVisibility();
    if (CurrentInventoryVisilbity == ESlateVisibility::Collapsed)
    {
        InventoryWidget->SetVisibility(ESlateVisibility::Visible);
        BackdropWidget->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
        BackdropWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
}

UUserWidget* USimpleRPGHUDWidget::GetInventoryWidget()
{
    return InventoryWidget;
}

UUserWidget* USimpleRPGHUDWidget::GetItemDescriptionWidget()
{
    return ItemDescriptionWidget;
}
