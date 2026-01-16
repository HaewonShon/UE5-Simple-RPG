// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleRPGHUDWidget.h"
#include "Inventory/InventoryWidget.h"
#include "BackdropWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Inventory/ItemDescriptionWidget.h"
#include "DialogueWidget.h"

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

    DialogueDisplayWidget = CreateWidget<UDialogueWidget>(GetOwningPlayer(), DialogueDisplayWidgetClass.Get());
    if (DialogueDisplayWidget)
    {
        DialogueDisplayWidget->SetVisibility(ESlateVisibility::Collapsed);
        UCanvasPanelSlot* DialogueDisplayWidgetSlot = MainCanvas->AddChildToCanvas(DialogueDisplayWidget);
        if (DialogueDisplayWidgetSlot)
        {
            constexpr float DIALOGUE_WIDGET_HEIGHT = 480.f;
            constexpr float DIALOGUE_WIDGET_MARGIN = 100.f;
            DialogueDisplayWidgetSlot->SetAlignment(FVector2D(0.5f, 1.f));
            DialogueDisplayWidgetSlot->SetAnchors(FAnchors(0.f, 1.f, 1.f, 1.f)); // X stretch
            DialogueDisplayWidgetSlot->SetOffsets(FMargin(DIALOGUE_WIDGET_MARGIN, -DIALOGUE_WIDGET_HEIGHT, DIALOGUE_WIDGET_MARGIN, 20.f));
        }
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

void USimpleRPGHUDWidget::OpenDialogueWidget()
{
    DialogueDisplayWidget->SetVisibility(ESlateVisibility::Visible);
}

void USimpleRPGHUDWidget::CloseDialogueWidget()
{
    DialogueDisplayWidget->SetVisibility(ESlateVisibility::Collapsed);
}

TWeakObjectPtr<UUserWidget> USimpleRPGHUDWidget::GetInventoryWidget() const
{
    return InventoryWidget;
}
