// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnSlotDragBegin, FSlotInfo);
DECLARE_DELEGATE_TwoParams(FOnSlotDrop, FSlotInfo, FSlotInfo);

UENUM()
enum class ESlotType
{
	Weapon,
	Helmet,
	Chest,
	Pants,
	Boots,
	Storage,
	Count UMETA(Hidden)
};

USTRUCT()
struct FSlotInfo
{
	GENERATED_BODY()

	ESlotType SlotType;
	int32 SlotIndex; // for ESlotType::Storage only
};

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

	int32 GetIndex() const;
	void SetIndex(int32 Index);

	class UTexture2D* GetIconTexture() const;

	FOnSlotDragBegin OnDragBegin;
	FOnSlotDrop OnDrop;

	UDragDropOperation* DragDropOperationRef;

	UPROPERTY(EditDefaultsOnly)
	ESlotType SlotType;

protected:
	/* drag-drop related events implementation */
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> ItemImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> StackText;

	// Slot Information
	int32 SlotIndex;

	bool bIsSlotFilled;

	FVector2f DragOffset;
};
