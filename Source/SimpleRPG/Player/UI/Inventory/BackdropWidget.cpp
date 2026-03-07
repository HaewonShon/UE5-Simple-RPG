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

bool UBackdropWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

    // 여기서 페이로드 검증 가능 (예: 아이템 드랍 허용 여부)
    // return; (Native 함수는 void인 경우가 많으므로 내부 로직 처리 후 시스템이 판단)
    UE_LOG(LogTemp, Warning, TEXT("NativeOnDragOver detected"));
    return true;
}