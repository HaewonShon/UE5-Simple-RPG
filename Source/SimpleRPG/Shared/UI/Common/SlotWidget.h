// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Shared/Common/SlotTypes.h"
#include "Shared/Common/ActionSlot.h"
#include "SlotWidget.generated.h"

DECLARE_DELEGATE(FOnSlotLeave);
DECLARE_DELEGATE_OneParam(FOnSlotEvent, const FSlotAddress&);
DECLARE_DELEGATE_TwoParams(FOnSlotDrop, const FSlotAddress&, const FSlotAddress&);

/**
 *		Common widget to display slot on screen.
 */
UCLASS()
class SIMPLERPG_API USlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FActionSlot SlotContent;

	FOnSlotEvent OnDragBegin;
	FOnSlotEvent OnDoubleClick;
	FOnSlotEvent OnHovered;
	FOnSlotDrop OnDrop;
	FOnSlotLeave OnHoverEnded;

	UDragDropOperation* DragDropOperationRef;

	void UpdateSlot(const FActionSlot& NewSlotContent);
	void ClearSlot();
	bool IsEmpty() const;
	void SetSlotAddreess(const FSlotAddress& NewAddress);

protected:
	/*** widget event overrides ***/
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& MouseEvent) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> QuantityText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBorder> Background;

	FSlotAddress Address;
	FLinearColor DefaultBackgroundColor;
};
