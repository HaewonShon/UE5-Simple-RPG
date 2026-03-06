// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Shared/Item/UI/ItemSlotWidget.h" // ESlotType
#include "ItemGridWidget.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnGridSlotLeave);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGridSlotEvent, const FSlotAddress&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGridSlotDrop, const FSlotAddress&, const FSlotAddress&);

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
	void OnSlotDragBegin(const FSlotAddress& SlotAddress);

	UFUNCTION()
	void OnSlotsSwapped(const FSlotAddress& SourceSlotAddress, const FSlotAddress& TargetSlotAddress);

	UFUNCTION()
	void OnSlotDoubleClicked(const FSlotAddress& SlotAddress);

	UFUNCTION()
	void OnSlotHovered(const FSlotAddress& SlotAddress);

	UFUNCTION()
	void OnSlotHoverEnded();

	UPROPERTY(EditAnywhere, Category = "Grid")
	int32 PageWidth;

	UPROPERTY(EditAnywhere, Category = "Grid")
	int32 PageHeight;

	UPROPERTY(EditDefaultsOnly, Category = "Grid")
	TSubclassOf<class USlotWidget> SlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Grid")
	TSubclassOf<class UItemSlotDragWidget> SlotVisualWidgetClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> SlotGridPanel;
};
