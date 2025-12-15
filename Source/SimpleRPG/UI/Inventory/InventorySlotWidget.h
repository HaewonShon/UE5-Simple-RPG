// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnDragBegin, class UInventorySlotWidget*);

/**
 *	 Widget for each item slot in inventory.
 */
UCLASS()
class SIMPLERPG_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/* Setter for Item Image and stack count text */
	void SetItem(const struct FItemInstance* Item);

	void ClearItem();

	FOnDragBegin OnDragBegin;

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> ItemImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> StackText;

	bool bIsSlotFilled;
	UDragDropOperation* DragDropOperationRef;
	FVector2f DragOffset;
};
