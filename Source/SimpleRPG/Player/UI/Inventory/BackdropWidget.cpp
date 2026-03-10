// Fill out your copyright notice in the Description page of Project Settings.


#include "BackdropWidget.h"
#include "Shared/UI/Common/SlotDragDropOp.h"

bool UBackdropWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    UE_LOG(LogTemp, Warning, TEXT("Backdrop detected"));
    bool Result = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
    if (USlotDragDropOp* DragDropOp = Cast<USlotDragDropOp>(InOperation))
    {
        OnItemDiscard.ExecuteIfBound(DragDropOp->SlotAddress);
        UE_LOG(LogTemp, Warning, TEXT("Item discard broadasted"));
    }
    return true;
}