// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../Character/Inventory/Inventory.h"
#include "InventorySlotWidget.h"
#include "InventoryWidget.generated.h"

/**
 *    Inventory Component for character, support per-page item add/use/sort, etc.
 */
UCLASS()
class SIMPLERPG_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UInventoryWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;

protected:
	/**************************
	 * Inventory Functions 
	 **************************/
	UFUNCTION()
	void OnInventoryToggled(ESlateVisibility ChangedVisibility);

	UFUNCTION()
	void OnPageSelected(int32 PageIndex);

	UFUNCTION()
	void OnCurrentPageSort();

	//UFUNCTION(BlueprintCallable)
	//void OnItemSelected();

	UFUNCTION()
	void OnSlotDragBegin(FSlotInfo SlotWidget);

	UFUNCTION()
	void OnSwapSlots(FSlotInfo Slot1, FSlotInfo Slot2);

protected:
	/* Update Inventory manually when interface opened */
	UFUNCTION()
	void OnContentChanged(EInventoryCategory ChangedPageCategory);

	UFUNCTION()
	void UpdateCurrentPageContents();


	/*
	*   Widget Properties 
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	int32 PageWidth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	int32 PageHeight;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<class UInventorySlotWidget> SlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<class UInventorySlotDragWidget> SlotVisualWidgetClass;

	/*
	*   Bind Widgets
	*/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UUniformGridPanel> SlotGridPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UInvalidationBox> InvalidationBox;

	// Equipment Slots
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UInventorySlotWidget> HelmetSlot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UInventorySlotWidget> ChestSlot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UInventorySlotWidget> PantsSlot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UInventorySlotWidget> BootsSlot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UInventorySlotWidget> WeaponSlot;
	

	/*
	*   Other members for inventory widget
	*/
	UPROPERTY()
	TWeakObjectPtr<class UInventoryComponent> InventoryComponent;

	UPROPERTY()
	EInventoryCategory SelectedPage;

	UPROPERTY()
	TMap<EInventoryCategory, bool> bIsPageContentChanged;

	UPROPERTY()
	TObjectPtr<class UInventorySlotDragWidget> SlotVisualWidget;

	UPROPERTY()
	TMap<ESlotType, class UInventorySlotWidget*> EquipmentSlotMap;
};
