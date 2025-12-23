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
        /*if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(InventoryWidget->Slot))
        {
            CanvasSlot->SetSize(FVector2D(800.f, 1200.f));
        }*/

        InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);

        UCanvasPanelSlot* InventorySlot = MainCanvas->AddChildToCanvas(InventoryWidget);
        const FVector2D InventoryWidgetSize{ 640.f, 900.f };
        //UE_LOG(LogTemp, Warning, TEXT("Inventory Widget size: %f, %f"), InventoryWidgetSize.X, InventoryWidgetSize.Y);

        InventorySlot->SetAnchors(FAnchors(1.f, 0.f));
        InventorySlot->SetAlignment(FVector2D(1.f, 0.f));
        InventorySlot->SetOffsets(FMargin(-(20.f), 20.f, InventoryWidgetSize.X, InventoryWidgetSize.Y));
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
