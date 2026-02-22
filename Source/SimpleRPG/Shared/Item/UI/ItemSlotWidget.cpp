// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSlotWidget.h"
#include "ItemDragDropOp.h"
#include "../ItemData.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UItemSlotWidget::SetItem(const FItemInstance& Item)
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

void UItemSlotWidget::ClearItem()
{
	ItemImage->SetBrushFromTexture(nullptr);
	StackText->SetText(FText());
	bIsSlotFilled = false;
}

int32 UItemSlotWidget::GetIndex() const
{
	return SlotIndex;
}

void UItemSlotWidget::SetIndex(int32 Index)
{
	SlotIndex = Index;
}

UTexture2D* UItemSlotWidget::GetIconTexture() const
{
	UObject* Resource = ItemImage->GetBrush().GetResourceObject();
	if (UTexture2D* Texture = Cast<UTexture2D>(Resource))
	{
		return Texture;
	}
	return nullptr;
}

void UItemSlotWidget::NativeConstruct()
{
	ClearItem();
}

FReply UItemSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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

void UItemSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	UE_LOG(LogTemp, Verbose, TEXT("Slot NativeOnDragDetected Detected"));

	UItemDragDropOp* DragDropOp = Cast<UItemDragDropOp>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemDragDropOp::StaticClass()));
	DragDropOp->DraggingSlot.SlotType = this->SlotType;
	DragDropOp->DraggingSlot.SlotIndex = GetIndex();
	OutOperation = DragDropOp;

	// Set DragDrop operation ref so Inventory Widget can set visual widget
	DragDropOperationRef = OutOperation;
	OnDragBegin.ExecuteIfBound(DragDropOp->DraggingSlot);
}

bool UItemSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Verbose, TEXT("Slot NativeOnDrop Detected"));

	DragDropOperationRef = nullptr;

	FSlotInfo SlotToSwap = Cast<UItemDragDropOp>(InOperation)->DraggingSlot;
	FSlotInfo Self;
	Self.SlotType = this->SlotType;
	Self.SlotIndex = GetIndex();
	OnDrop.ExecuteIfBound(SlotToSwap, Self);

	return true;
}

FReply UItemSlotWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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

void UItemSlotWidget::NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
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

void UItemSlotWidget::NativeOnMouseLeave(const FPointerEvent& MouseEvent)
{
	Super::NativeOnMouseLeave(MouseEvent);

	if (!bIsSlotFilled)
	{
		return;
	}

	OnHoverEnded.ExecuteIfBound();
}
