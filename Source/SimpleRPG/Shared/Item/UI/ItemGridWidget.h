// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Shared/Item/UI/ItemSlotWidget.h" // ESlotType
#include "ItemGridWidget.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnGridSlotLeave);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGridSlotEvent, FSlotInfo);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGridSlotDrop, FSlotInfo, FSlotInfo);

/**
 *		A Common widget to display item grid.
 */
UCLASS()
class SIMPLERPG_API UItemGridWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	class UItemSlotWidget* GetSlotAt(int32 SlotIndex);
	TArray<UWidget*> GetAllSlots();
	void SetSlotType(ESlotType Type);

	FOnGridSlotEvent OnDragBegin;
	FOnGridSlotEvent OnDoubleClick;
	FOnGridSlotEvent OnHovered;
	FOnGridSlotDrop OnDrop;
	FOnGridSlotLeave OnHoverEnded;

protected:
	/** Slot Events **/
	UFUNCTION()
	void OnSlotDragBegin(FSlotInfo SlotWidget);

	UFUNCTION()
	void OnSlotsSwapped(FSlotInfo Slot1, FSlotInfo Slot2);

	UFUNCTION()
	void OnSlotDoubleClicked(FSlotInfo SlotWidget);

	UFUNCTION()
	void OnSlotHovered(FSlotInfo SlotWidget);

	UFUNCTION()
	void OnSlotHoverEnded();

	UPROPERTY(EditAnywhere, Category = "Grid")
	int32 PageWidth;

	UPROPERTY(EditAnywhere, Category = "Grid")
	int32 PageHeight;

	UPROPERTY(EditDefaultsOnly, Category = "Grid")
	TSubclassOf<class UItemSlotWidget> SlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Grid")
	TSubclassOf<class UItemSlotDragWidget> SlotVisualWidgetClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> SlotGridPanel;
};
