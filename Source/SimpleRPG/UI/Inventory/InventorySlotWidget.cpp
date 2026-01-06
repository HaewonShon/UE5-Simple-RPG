// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlotWidget.h"
#include "Components/Image.h"
#include "../../Item/ItemData.h"
#include "Components/TextBlock.h"
#include "InventoryDragDropOp.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UInventorySlotWidget::SetItem(const FItemInstance& Item)
{
	if (!Item.ItemData)
	{
		return;
	}

	ItemImage->SetBrushFromTexture(Item.ItemData->Icon);
	if (Item.StackCount > 1)
	{
		StackText->SetText(FText::AsNumber(Item.StackCount));
	}
	else
	{
		StackText->SetText(FText::GetEmpty());
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

	if (Reply.NativeReply.IsEventHandled() || InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton || !bIsSlotFilled)
	{
		return Reply.NativeReply;
	}

	Reply.NativeReply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	return Reply.NativeReply;
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	UE_LOG(LogTemp, Verbose, TEXT("Slot NativeOnDragDetected Detected"));

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
	UE_LOG(LogTemp, Verbose, TEXT("Slot NativeOnDrop Detected"));

	DragDropOperationRef = nullptr;

	FSlotInfo SlotToSwap = Cast<UInventoryDragDropOp>(InOperation)->DraggingSlot;
	FSlotInfo Self;
	Self.SlotType = this->SlotType;
	Self.SlotIndex = GetIndex();
	OnDrop.ExecuteIfBound(SlotToSwap, Self);

	return true;
}

FReply UInventorySlotWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Verbose, TEXT("Slot NativeOnDoubleClick Detected"));

	FEventReply Reply;
	Reply.NativeReply = Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
	if (Reply.NativeReply.IsEventHandled() || !bIsSlotFilled)
	{
		return Reply.NativeReply;
	}

	FSlotInfo Self;
	Self.SlotType = this->SlotType;
	Self.SlotIndex = GetIndex();

	OnDoubleClick.ExecuteIfBound(Self);

	return FReply::Handled();
}

void UInventorySlotWidget::NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	Super::NativeOnMouseEnter(MyGeometry, MouseEvent);

	if (!bIsSlotFilled)
	{
		return;
	}

	FSlotInfo Self;
	Self.SlotType = this->SlotType;
	Self.SlotIndex = GetIndex();
	OnHovered.ExecuteIfBound(Self);
}

void UInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& MouseEvent)
{
	Super::NativeOnMouseLeave(MouseEvent);

	if (!bIsSlotFilled)
	{
		return;
	}

	OnHoverEnded.ExecuteIfBound();
}
