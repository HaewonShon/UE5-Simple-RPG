// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/UI/Common/SlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Player/SimpleRPGPlayerState.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Shared/UI/UISubsystem.h"
#include "SlotDragDropOp.h"
#include "SlotDragWidget.h"

void USlotWidget::UpdateSlot(const FSlotContent& NewSlotContent)
{
	SlotContent = NewSlotContent;
	SetSlotAddress(SlotContent.SlotAddress);

	Icon->SetBrushFromTexture(SlotContent.ContentAsset->Icon.Get());
	Icon->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));

	this->InvalidateLayoutAndVolatility(); // Refresh InvalidationBox cache
}

void USlotWidget::ClearSlot()
{
	SlotContent = FSlotContent();
	//Icon->SetBrushFromTexture(nullptr);
	Icon->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.f));
}

bool USlotWidget::IsEmpty() const
{
	return SlotContent.IsEmpty();
}

void USlotWidget::SetSlotAddress(const FSlotAddress& NewAddress)
{
	Address = NewAddress;
}

void USlotWidget::NativeConstruct()
{
}

FReply USlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply Reply;
	Reply.NativeReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (Reply.NativeReply.IsEventHandled() || InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton || IsEmpty())
	{
		return Reply.NativeReply;
	}

	Reply.NativeReply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	return Reply.NativeReply;
}

void USlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	UE_LOG(LogTemp, Verbose, TEXT("Slot NativeOnDragDetected Detected"));

	if (SlotContent.IsEmpty())
	{
		return;
	}

	USlotDragDropOp* DragDropOp = Cast<USlotDragDropOp>(UWidgetBlueprintLibrary::CreateDragDropOperation(USlotDragDropOp::StaticClass()));
	DragDropOp->SlotAddress = this->Address;

	if (UUISubsystem* UISubsystem = GetOwningLocalPlayer()->GetSubsystem<UUISubsystem>())
	{
		USlotDragWidget* SlotVisualWidget = UISubsystem->GetSlotDragWidget();
		SlotVisualWidget->OnDragBegin(GetIconTexture());
		SlotVisualWidget->SetDesiredSize(FVector2D{ 64.f, 64.f });
		DragDropOp->DefaultDragVisual = SlotVisualWidget;
	}

	OutOperation = DragDropOp;

	// Set DragDrop operation ref so Inventory Widget can set visual widget
	DragDropOperationRef = OutOperation;
	OnDragBegin.ExecuteIfBound(DragDropOp->SlotAddress);
}

bool USlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Verbose, TEXT("Slot NativeOnDrop Detected"));

	DragDropOperationRef = nullptr;

	FSlotAddress SlotToSwap = Cast<USlotDragDropOp>(InOperation)->SlotAddress;
	OnDrop.ExecuteIfBound(SlotToSwap, this->Address);

	return true;
}

FReply USlotWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Verbose, TEXT("Slot NativeOnDoubleClick Detected"));

	FEventReply Reply;
	Reply.NativeReply = Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
	if (Reply.NativeReply.IsEventHandled() || IsEmpty())
	{
		return Reply.NativeReply;
	}

	if (SlotContent.ContentAsset.IsValid() && SlotContent.ContentAsset->CanExecute(GetOwningPlayerState<ASimpleRPGPlayerState>(), Address))
	{
		SlotContent.ContentAsset->Execute(GetOwningPlayerState<ASimpleRPGPlayerState>(), Address);
	}

//	OnDoubleClick.ExecuteIfBound(Address);
	return FReply::Handled();
}

void USlotWidget::NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	UE_LOG(LogTemp, Verbose, TEXT("Slot NativeOnMouseEnter Detected"));
	Super::NativeOnMouseEnter(MyGeometry, MouseEvent);

	if (IsEmpty())
	{
		return;
	}
	OnHovered.ExecuteIfBound(Address);
}

void USlotWidget::NativeOnMouseLeave(const FPointerEvent& MouseEvent)
{
	Super::NativeOnMouseLeave(MouseEvent);
	if (IsEmpty())
	{
		return;
	}
	OnHoverEnded.ExecuteIfBound();
}

UTexture2D* USlotWidget::GetIconTexture() const
{
	return Cast<UTexture2D>(Icon->GetBrush().GetResourceObject());
}