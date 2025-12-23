// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlotWidget.h"
#include "Components/Image.h"
#include "../../Item/ItemData.h"
#include "Components/TextBlock.h"
#include "InventoryDragDropOp.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UInventorySlotWidget::SetItem(const FItemInstance* Item)
{
	if (!Item)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Set Item image"));

	ItemImage->SetBrushFromTexture(Item->ItemData->Icon);
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
	StackText->SetText(FText());
	bIsSlotFilled = false;
}

int32 UInventorySlotWidget::GetIndex() const
{
	return SlotIndex;
}

void UInventorySlotWidget::SetIndex(int32 Index)
{
	SlotIndex = Index;
}

UTexture2D* UInventorySlotWidget::GetIconTexture() const
{
	UObject* Resource = ItemImage->GetBrush().GetResourceObject();
	if (UTexture2D* Texture = Cast<UTexture2D>(Resource))
	{
		return Texture;
	}
	return nullptr;
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply Reply;
	Reply.NativeReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return Reply.NativeReply;
	}

	DragOffset = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	Reply.NativeReply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	return Reply.NativeReply;
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (!bIsSlotFilled)
	{
		return;
	}

	UInventoryDragDropOp* DragDropOp = Cast<UInventoryDragDropOp>(UWidgetBlueprintLibrary::CreateDragDropOperation(UInventoryDragDropOp::StaticClass()));
	DragDropOp->DraggingSlot.SlotType = this->SlotType;
	DragDropOp->DraggingSlot.SlotIndex = GetIndex();
	OutOperation = DragDropOp;

	// Set DragDrop operation ref so Inventory Widget can set visual widget
	DragDropOperationRef = OutOperation;
	OnDragBegin.ExecuteIfBound(DragDropOp->DraggingSlot);
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	DragDropOperationRef = nullptr;

	FSlotInfo SlotToSwap = Cast<UInventoryDragDropOp>(InOperation)->DraggingSlot;
	FSlotInfo Self;
	Self.SlotType = this->SlotType;
	Self.SlotIndex = GetIndex();
	OnDrop.ExecuteIfBound(SlotToSwap, Self);

	UE_LOG(LogTemp, Log, TEXT("Slot NativeOnDrop Detected"));
	return true;
}
