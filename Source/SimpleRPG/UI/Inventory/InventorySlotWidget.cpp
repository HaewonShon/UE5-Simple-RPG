// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlotWidget.h"
#include "Components/Image.h"
#include "../../Item/ItemData.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UInventorySlotWidget::SetItem(const FItemInstance* Item)
{
	if (!Item)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Set Item image"));

	ItemImage->SetBrushFromTexture(Item->ItemData->Icon);
	//ItemImage->SetBrushTintColor(FSlateColor(FColor::White));
	if (Item->StackCount > 1)
	{
		StackText->SetText(FText::AsNumber(Item->StackCount));
	}

	this->InvalidateLayoutAndVolatility(); // Refresh InvalidationBox cache
	bIsSlotFilled = true;
}

void UInventorySlotWidget::ClearItem()
{
	ItemImage->SetBrushFromTexture(nullptr);
	//ItemImage->SetBrushTintColor(FSlateColor(FColor::Transparent));
	StackText->SetText(FText());
	bIsSlotFilled = false;
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton().ToString() != FString("LeftMouseButton"))
	{
		return;
	}

	DragOffset = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::RightMouseButton).NativeReply;
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (!bIsSlotFilled)
	{
		return;
	}

	// Set DragDrop operation ref so Inventory Widget can set visual widget
	DragDropOperationRef = OutOperation;
	OnDragBegin.ExecuteIfBound(this);
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	DragDropOperationRef = nullptr;
	return false;
}
