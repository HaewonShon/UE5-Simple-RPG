// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/UI/Common/SessionWidget.h"
#include "Player/Components/InventoryComponent.h"
#include "Shared/Item/UI/ItemSlotWidget.h"
#include "InventoryWidget.generated.h"

DECLARE_DELEGATE(FOnInventoryToggleRequest);

/**
 *    Inventory Component for character, support per-page item add/use/sort, etc.
 */
UCLASS()
class SIMPLERPG_API UInventoryWidget : public USessionWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void BindItemDiscardDelegate(class UBackdropWidget* BackdropWidget);
	void SetDescriptionWidgetRef(class UItemDescriptionWidget* DescriptionWidgetRef);

protected:
	/**************************
	 * Inventory Functions 
	 **************************/
	UFUNCTION()
	void OnInventoryToggled(ESlateVisibility ChangedVisibility);

	UFUNCTION()
	void OnCurrentPageSort();

	UFUNCTION()
	void OnSlotDragBegin(const FSlotAddress& SlotAddress);

	UFUNCTION()
	void OnSlotsSwapped(const FSlotAddress& SourceSlotAddress, const FSlotAddress& TargetSlotAddress);

	UFUNCTION()
	void OnItemDiscarded(const FSlotAddress& SlotAddress);

	UFUNCTION()
	void OnItemUsed(const FSlotAddress& SlotAddress);

	UFUNCTION()
	void OnSlotHovered(const FSlotAddress& SlotAddress);

	UFUNCTION()
	void OnSlotHoverEnded();

protected:
	/* Update Inventory manually when interface opened */
	UFUNCTION()
	void UpdateContents();

	UFUNCTION()
	void UpdateEquipmentSlotWidgets();

	UFUNCTION()
	void UpdateCurrentPageContents();

	UFUNCTION()
	void UpdateGoldAmount(int32 Amount);

	UFUNCTION()
	void OnCloseButtonClicked();


	/**************************
	*   Widget Properties 
	***************************/

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<class UItemSlotWidget> SlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<class UItemSlotDragWidget> SlotVisualWidgetClass;

	//UPROPERTY()
	//TMap<ESlotType, class UItemSlotWidget*> EquipmentSlotMap;

	/************************
	*   Bind Widgets
	*************************/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UItemGridWidget> GridWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBackdropWidget> BackdropWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UInvalidationBox> InvalidationBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> GoldDisplayText;

	///**** Equipment Slots ****/
	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<class UItemSlotWidget> HelmetSlot;

	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<class UItemSlotWidget> ChestSlot;

	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<class UItemSlotWidget> PantsSlot;

	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<class UItemSlotWidget> BootsSlot;

	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<class UItemSlotWidget> WeaponSlot;

	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<class UButton> CloseButton;


	/************************
	*   Other members for inventory widget
	*************************/
	UPROPERTY()
	TWeakObjectPtr<class UInventoryComponent> InventoryComponentRef;

	UPROPERTY()
	TMap<EInventoryCategory, bool> bIsPageContentChanged;

	UPROPERTY()
	TObjectPtr<class UItemSlotDragWidget> SlotVisualWidget;

	UPROPERTY()
	TWeakObjectPtr<class UItemDescriptionWidget> ItemDescriptionWidgetRef;
};
