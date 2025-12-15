// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRPGHUDWidget.h"
#include "Inventory/InventoryWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void USimpleRPGHUDWidget::NativeConstruct()
{ 
    InventoryWidget = CreateWidget<UInventoryWidget>(GetWorld(), InventoryWidgetClass.Get());
    if (InventoryWidget)
    {
        if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(InventoryWidget->Slot))
        {
            CanvasSlot->SetSize(FVector2D(800.f, 1200.f));
        }

        InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);

        UCanvasPanelSlot* InventorySlot = MainCanvas->AddChildToCanvas(InventoryWidget);

        InventorySlot->SetAnchors(FAnchors(1.f, 1.f));
        InventorySlot->SetAlignment(FVector2D(1.f, 1.f));
        InventorySlot->SetOffsets(FMargin(-20.f, -20.f, 640, 900));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to create Inventory Widget"));
    }
}

void USimpleRPGHUDWidget::ToggleInventory()
{
    if (InventoryWidget)
    {
        ESlateVisibility CurrentInventoryVisilbity = InventoryWidget->GetVisibility();
        if (CurrentInventoryVisilbity == ESlateVisibility::Collapsed)
        {
            InventoryWidget->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

TWeakObjectPtr<UUserWidget> USimpleRPGHUDWidget::GetInventoryWidget() const
{
    return InventoryWidget;
}
