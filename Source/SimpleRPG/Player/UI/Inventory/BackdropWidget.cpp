// Fill out your copyright notice in the Description page of Project Settings.


#include "BackdropWidget.h"
#include "Shared/Item/UI/ItemDragDropOp.h"

bool UBackdropWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    UE_LOG(LogTemp, Verbose, TEXT("Backdrop detected"));
    bool Result = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
    if (UItemDragDropOp* DragDropOp = Cast<UItemDragDropOp>(InOperation))
    {
        OnItemDiscard.ExecuteIfBound(DragDropOp->DraggingSlot);
        UE_LOG(LogTemp, Verbose, TEXT("Item discard broadasted"));
    }
    return true;
}